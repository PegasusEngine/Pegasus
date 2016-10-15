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
            Blocks : []
        }, {
            type : Lane,
            name : "",
            Blocks : []
        }, {
            type : Lane,
            name : "",
            Blocks : []
        }, {
            type : Lane,
            name : "",
            Blocks : []
        }, {
            type : Lane,
            name : "",
            Blocks : []
        }, {
            type : Lane,
            name : "",
            Blocks : []
        }, {
            type : Lane,
            name : "",
            Blocks : [{
                    type : Block,
                    props : {
                        Beat : 3578,
                        Duration : 2048,
                        Name : PsyBeads,
                        Color : [128, 0, 56],
                        _o_p_ : {
                            __schema__ : {
                                sizetypes : [],
                                names : [],
                                typeNames : []
                            }
                        },
                        script : {@BlockScripts/test.bs}
                    }
                }]
        }, {
            type : Lane,
            name : "",
            Blocks : []
        }, {
            type : Lane,
            name : "",
            Blocks : [{
                    type : TextureTestBlock,
                    props : {
                        Beat : 2708,
                        Duration : 2304,
                        Name : "",
                        Color : [29, 99, 10],
                        _o_p_ : {
                            __schema__ : {
                                sizetypes : [],
                                names : [],
                                typeNames : []
                            }
                        }
                    }
                }]
        }, {
            type : Lane,
            name : Camera,
            Blocks : [{
                    type : FractalCube2Block,
                    props : {
                        Beat : 4172,
                        Duration : 1024,
                        Name : "",
                        Color : [58, 140, 249],
                        _o_p_ : {
                            __schema__ : {
                                sizetypes : [],
                                names : [],
                                typeNames : []
                            }
                        }
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
                        Name : Test,
                        Color : [228, 207, 245],
                        _o_p_ : {
                            StripThickness : 55.000000,
                            StripAA : 6.000000,
                            Light1 : [2.000000, 0.400000, 0.500000, 1.000000],
                            Light2 : [2.000000, 0.400000, 0.900000, 1.000000],
                            Light3 : [20.000000, 4.400000, 10.300000, 1.000000],
                            __schema__ : {
                                sizetypes : [393232, 393232, 393232, 196612, 196612],
                                names : [Light1, Light2, Light3, StripThickness, StripAA],
                                typeNames : [float4, float4, float4, float, float]
                            }
                        },
                        script : {@BlockScripts/basketball.bs}
                    }
                }, {
                    type : Block,
                    props : {
                        Beat : 2048,
                        Duration : 1024,
                        Name : Test2,
                        Color : [0, 100, 100],
                        _o_p_ : {
                            SphereLayerCount : 4,
                            SphereRadius : 0.450000,
                            Material_GlitterIntensity : 0.070000,
                            CubeRotAxis : [0.300000, 0.400000, 0.010000],
                            Material_GlitterRepetition : [20.100000, 20.100000],
                            __schema__ : {
                                sizetypes : [196612, 327692, 65540, 196612, 262152],
                                names : [SphereRadius, CubeRotAxis, SphereLayerCount, Material_GlitterIntensity, Material_GlitterRepetition],
                                typeNames : [float, float3, int, float, float2]
                            }
                        },
                        script : {@BlockScripts/geometrytestblock.bs}
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
                        Name : "",
                        Color : [239, 248, 111],
                        _o_p_ : {
                            __schema__ : {
                                sizetypes : [],
                                names : [],
                                typeNames : []
                            }
                        }
                    }
                }]
        }, {
            type : Lane,
            name : Blob,
            Blocks : [{
                    type : Block,
                    props : {
                        Beat : 1010,
                        Duration : 1024,
                        Name : BlockSegment,
                        Color : [144, 244, 123],
                        _o_p_ : {
                            SphereCenter : [0.000000, 0.000000, -4.000000],
                            __schema__ : {
                                sizetypes : [327692],
                                names : [SphereCenter],
                                typeNames : [float3]
                            }
                        },
                        script : {@BlockScripts/partydemo.bs}
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
        }],
    properties : {
        Name : "",
        _o_p_ : {
            signature : 1,
            Fov : 2.500000,
            CamPos : [0.000000, 5.000000, 13.000000],
            CamDir : [0.000000, -0.100000, -1.000000],
            __schema__ : {
                sizetypes : [65540, 327692, 327692, 196612],
                names : [signature, CamPos, CamDir, Fov],
                typeNames : [int, float3, float3, float]
            }
        }
    },
    master-script : {@Timeline/masterScript.bs}
}