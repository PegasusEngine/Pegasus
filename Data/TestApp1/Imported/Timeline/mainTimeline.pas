{
    __type_guid__ : 1414086995,
    num-beats : 128,
    ticks-per-beat : 128,
    beats-per-minute-bin : 1124728832,
    beats-per-minute-float : 138.000000,
    typename : Timeline,
    lanes : [{
            type : Lane,
            name : "",
            Blocks : [{
                    type : Block,
                    properties : {
                        Beat : 2042,
                        Duration : 2048,
                        script : BlockScripts/test.bs
                    },
                    editor-props : {
                        color : 12615935
                    }
                }]
        }, {
            type : Lane,
            name : "",
            Blocks : [{
                    type : FractalCube2Block,
                    properties : {
                        Beat : 2030,
                        Duration : 1024
                    },
                    editor-props : {
                        color : 12648320
                    }
                }]
        }, {
            type : Lane,
            name : "",
            Blocks : [{
                    type : TextureTestBlock,
                    properties : {
                        Beat : 2289,
                        Duration : 2304
                    },
                    editor-props : {
                        color : 8454080
                    }
                }]
        }, {
            type : Lane,
            name : Camera,
            Blocks : [{
                    type : CameraLineBlock,
                    properties : {
                        Beat : 0,
                        Duration : 1024
                    },
                    editor-props : {
                        color : 65535
                    }
                }]
        }, {
            type : Lane,
            name : Beads,
            Blocks : [{
                    type : Block,
                    properties : {
                        Beat : 0,
                        Duration : 1024,
                        script : BlockScripts/partydemo.bs
                    },
                    editor-props : {
                        color : 8421631
                    }
                }, {
                    type : Block,
                    properties : {
                        Beat : 1024,
                        Duration : 1024,
                        script : BlockScripts/geometrytestblock.bs
                    },
                    editor-props : {
                        color : 1849107
                    }
                }]
        }, {
            type : Lane,
            name : Fractal,
            Blocks : [{
                    type : FractalCubeBlock,
                    properties : {
                        Beat : 4098,
                        Duration : 768
                    },
                    editor-props : {
                        color : 8454016
                    }
                }]
        }, {
            type : Lane,
            name : Blob,
            Blocks : []
        }, {
            type : Lane,
            name : "",
            Blocks : []
        }, {
            type : Lane,
            name : "",
            Blocks : []
        }]
}