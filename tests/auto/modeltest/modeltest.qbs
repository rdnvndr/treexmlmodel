import "../main.qbs" as SharedProduct

SharedProduct {
    name: "modeltest"
    type: "staticlibrary"

    productname: ""
    version:     "1.0.0"
    description: ""
    company:     "RTPTechGroup"
    copyright:   "Copyright (C) RTPTechGroup"

    Depends { name: "Qt"; submodules: ["widgets", "testlib"] }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory + "/../"
    }

    files: [
        "*.h",
        "*.cpp",
        "*.ui",
        "*.qrc"
    ]
}
