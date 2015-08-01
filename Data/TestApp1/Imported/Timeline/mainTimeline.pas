{
    num-beats : 128,
    ticks-per-beat : 128,
    beats-per-minute-bin : 1124728832,
    beats-per-minute-float : 138.000000,
    type : Timeline,
    lanes : [{
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
                        script : BlockScripts/test.bs
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
                }, {
                    type : Block,
                    properties : {
                        Beat : 2048,
                        Duration : 2048,
                        script : BlockScripts/test.bs
                    },
                    editor-props : {
                        color : 12615935
                    }
                }]
        }, {
            type : Lane,
            name : Fractal,
            Blocks : [{
                    type : FractalCube2Block,
                    properties : {
                        Beat : 2048,
                        Duration : 1024
                    },
                    editor-props : {
                        color : 12648320
                    }
                }, {
                    type : FractalCubeBlock,
                    properties : {
                        Beat : 5376,
                        Duration : 768
                    },
                    editor-props : {
                        color : 8454016
                    }
                }]
        }, {
            type : Lane,
            name : Blob,
            Blocks : [{
                    type : TextureTestBlock,
                    properties : {
                        Beat : 3072,
                        Duration : 2304
                    },
                    editor-props : {
                        color : 8454080
                    }
                }]
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
