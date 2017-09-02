import "../main.qbs" as DocProduct

DocProduct {
    name: "treexmlmodel-api"
    type: "docs"

    docName: "treexmlmodel-dev"
    Group {
        fileTagsFilter: ["docs"]
        qbs.install: true
        qbs.installDir: product.docPath
    }
}
