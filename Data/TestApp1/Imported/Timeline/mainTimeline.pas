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
                    props : {
                        Beat : 2042,
                        Duration : 2048,
                        script : BlockScripts/test.bs,
                        Color : [128, 0, 56]
                    }
                }]
        }, {
            type : Lane,
            name : "",
            Blocks : [{
                    type : FractalCube2Block,
                    props : {
                        Beat : 2030,
                        Duration : 1024,
                        Color : [58, 140, 249]
                    }
                }]
        }, {
            type : Lane,
            name : "",
            Blocks : [{
                    type : TextureTestBlock,
                    props : {
                        Beat : 2289,
                        Duration : 2304,
                        Color : [29, 99, 10]
                    }
                }]
        }, {
            type : Lane,
            name : Camera,
            Blocks : [{
                    type : CameraLineBlock,
                    props : {
                        Beat : 0,
                        Duration : 1024,
                        Color : [240, 0, 240]
                    }
                }]
        }, {
            type : Lane,
            name : Beads,
            Blocks : [{
                    type : Block,
                    props : {
                        Beat : 0,
                        Duration : 1024,
                        script : BlockScripts/partydemo.bs,
                        Color : [144, 244, 123]
                    }
                }, {
                    type : Block,
                    props : {
                        Beat : 1024,
                        Duration : 1024,
                        script : BlockScripts/geometrytestblock.bs,
                        Color : [0, 100, 100]
                    }
                }]
        }, {
            type : Lane,
            name : Fractal,
            Blocks : [{
                    type : FractalCubeBlock,
                    props : {
                        Beat : 4098,
                        Duration : 768,
                        Color : [239, 248, 111]
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
