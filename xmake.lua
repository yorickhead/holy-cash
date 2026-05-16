add_rules("mode.debug", "mode.release")

-- ====================== Общая библиотека ======================
target("hclib")
set_kind("static")
add_files("src/*.c")
add_includedirs("src", { public = true })

-- ====================== Client ======================
target("hcclient")
set_kind("binary")
add_deps("hclib")
add_files("src/client.c")
-- ====================== Server ======================

target("hcserver")
set_kind("binary")
add_deps("hclib")
add_files("src/server.c")
