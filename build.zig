const std = @import("std");
const PkgConfig = std.build.PkgConfig;

fn addEnvPaths(b: *std.Build, exe: *std.Build.Step.Compile, env: []const u8, is_lib: bool) void {
    const raw = std.process.getEnvVarOwned(b.allocator, env) catch return;
    defer b.allocator.free(raw);
    var it = std.mem.splitScalar(u8, raw, ';');
    while (it.next()) |seg| {
        if (seg.len == 0) continue;
        const p = b.dupe(seg);
        if (is_lib) exe.addLibraryPath(.{ .path = p })
        else exe.addIncludePath(.{ .path = p });
    }
}

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const link_math = target.os_tag != .windows;

    const common_sources = &[_][]const u8{
        "src/datastructs.c",
        "src/limits.c",
        "src/orders.c",
        "src/bst.c",
        "src/utils.c",
    };

    const include_paths = &[_][]const u8{"src", "thirdparty/imgui", "thirdparty/implot"};

    const exe = b.addExecutable(.{
        .name = "HFT_Orderbook",
        .target = target,
        .optimize = optimize,
    });
    for (include_paths) |p| {
        exe.addIncludePath(.{ .path = p });
    }
    for (common_sources) |src_file| {
        exe.addCSourceFile(.{ .file = .{ .path = src_file }, .flags = &[_][]const u8{} });
    }
    exe.addCSourceFile(.{ .file = .{ .path = "src/main.c" }, .flags = &[_][]const u8{} });
    exe.addCSourceFile(.{ .file = .{ .path = "src/benchmark.c" }, .flags = &[_][]const u8{} });
    exe.addCSourceFile(.{ .file = .{ .path = "src/binance.cpp" }, .flags = &[_][]const u8{"-std=c++17"} });
    exe.addCSourceFile(.{ .file = .{ .path = "src/viz.cpp" }, .flags = &[_][]const u8{"-std=c++17"} });
    // ImGui sources
    const imgui_sources = &[_][]const u8{
        "thirdparty/imgui/imgui.cpp",
        "thirdparty/imgui/imgui_draw.cpp",
        "thirdparty/imgui/imgui_tables.cpp",
        "thirdparty/imgui/imgui_widgets.cpp",
        "thirdparty/imgui/imgui_impl_sdl2.cpp",
        "thirdparty/imgui/imgui_impl_sdlrenderer2.cpp",
    };
    for (imgui_sources) |s| {
        exe.addCSourceFile(.{ .file = .{ .path = s }, .flags = &[_][]const u8{"-std=c++17"} });
    }
    // ImPlot sources
    exe.addCSourceFile(.{ .file = .{ .path = "thirdparty/implot/implot.cpp" }, .flags = &[_][]const u8{"-std=c++17"} });
    exe.addCSourceFile(.{ .file = .{ .path = "thirdparty/implot/implot_items.cpp" }, .flags = &[_][]const u8{"-std=c++17"} });
    exe.addCSourceFile(.{ .file = .{ .path = "src/CuTest.c" }, .flags = &[_][]const u8{} });
    exe.addCSourceFile(.{ .file = .{ .path = "src/testCases.c" }, .flags = &[_][]const u8{} });
    exe.linkLibC();
    exe.linkLibCpp();
    if (target.os_tag == .windows) {
        addEnvPaths(b, exe, "INCLUDE", false);
        addEnvPaths(b, exe, "LIB", true);
        exe.linkSystemLibrary("libcurl");
        exe.linkSystemLibrary("SDL2");
        exe.linkSystemLibrary("SDL2main");
    } else {
        const curl_pkg = PkgConfig.find(b, .{ .name = "libcurl" }) catch |err| {
            std.log.err("libcurl not found: {}", .{err});
            std.process.exit(1);
        };
        curl_pkg.addTo(exe);

        const sdl_pkg = PkgConfig.find(b, .{ .name = "sdl2" }) catch |err| {
            std.log.err("SDL2 not found: {}", .{err});
            std.process.exit(1);
        };
        sdl_pkg.addTo(exe);

        if (link_math) exe.linkSystemLibrary("m");
    }
    b.installArtifact(exe);

    const test_step = b.step("test", "Run C unit tests");
    const run_tests = b.addRunArtifact(exe);
    run_tests.addArg("--test");
    test_step.dependOn(&run_tests.step);

    const bench_step = b.step("benchmark", "Run performance benchmark");
    const run_bench = b.addRunArtifact(exe);
    run_bench.addArgs(&[_][]const u8{"--benchmark", "100000"});
    bench_step.dependOn(&run_bench.step);
}
