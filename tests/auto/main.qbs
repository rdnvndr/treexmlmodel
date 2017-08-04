import "../main.qbs" as TestProduct
TestProduct {
    name: "auto"
    type: "application"
    Group {
        fileTagsFilter: ["application"]
        qbs.install: true
        qbs.installDir: "tests/auto/treexmlmodel/"
    }
}
