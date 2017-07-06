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
                        Beat : 0,
                        Duration : 2048,
                        Name : TempleLights,
                        Color : [203, 231, 18],
                        _o_p_ : {
                            sphereLight1Intensity : 2.000000,
                            spotLight1Intensity : 2.000000,
                            spotLight1PosRad : [-48.000000, 66.900002, 46.000000, 62.000000],
                            spotLight1DirSpread : [1.000000, -1.500000, -1.000000, 54.000000],
                            sphereLight1PosRad : [62.500000, 33.500000, 6.500000, 32.000000],
                            sphereLight1Col : [255, 226, 61],
                            spotLight1Col : [75, 69, 255],
                            __schema__ : {
                                sizetypes : [393232, 393232, 393232, 458755, 458755, 196612, 196612],
                                names : [spotLight1PosRad, spotLight1DirSpread, sphereLight1PosRad, sphereLight1Col, spotLight1Col, sphereLight1Intensity, spotLight1Intensity],
                                typeNames : [float4, float4, float4, ubyte3, ubyte3, float, float]
                            }
                        },
                        script : {@BlockScripts/TempleSceneLights.bs}
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
                    type : Block,
                    props : {
                        Beat : 0,
                        Duration : 2048,
                        Name : DeferredLightingBegin,
                        Color : [240, 179, 148],
                        _o_p_ : {
                            __schema__ : {
                                sizetypes : [],
                                names : [],
                                typeNames : []
                            }
                        },
                        script : {@RenderSystems/Lighting/DeferredPassBeginBlock.bs}
                    }
                }]
        }, {
            type : Lane,
            name : "",
            Blocks : [{
                    type : Block,
                    props : {
                        Beat : 0,
                        Duration : 2048,
                        Name : TempleModel,
                        Color : [128, 128, 128],
                        _o_p_ : {
                            modelPosition : [0.000000, 0.000000, 0.000000],
                            modelQuaternion : [0.000000, 0.000000, 1.000000, 0.000000],
                            modelScale : [1.000000, 1.000000, 1.000000],
                            __schema__ : {
                                sizetypes : [327692, 393232, 327692],
                                names : [modelPosition, modelQuaternion, modelScale],
                                typeNames : [float3, float4, float3]
                            }
                        },
                        script : {@BlockScripts/TempleModel.bs}
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
                        Beat : 3381,
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
                        Beat : 4845,
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
                        Beat : 2443,
                        Duration : 1024,
                        Name : Test2,
                        Color : [0, 100, 100],
                        _o_p_ : {
                            SphereLayerCount : 4,
                            SphereRadius : 0.450000,
                            Material_GlitterIntensity : 0.070000,
                            CubeRotAxis : [0.300000, 0.400000, 0.010000],
                            Material_GlitterRepetition : [20.100000, 20.100000],
                            CombTranslation : [0.000000, 0.000000, 0.000000],
                            CombRotation : [1.000000, 0.000000, 0.000000, 0.000000],
                            CombScale : [1.000000, 1.000000, 1.000000],
                            __schema__ : {
                                sizetypes : [196612, 327692, 65540, 196612, 262152, 327692, 393232, 327692],
                                names : [SphereRadius, CubeRotAxis, SphereLayerCount, Material_GlitterIntensity, Material_GlitterRepetition, CombTranslation, CombRotation, CombScale],
                                typeNames : [float, float3, int, float, float2, float3, float4, float3]
                            }
                        },
                        script : {@BlockScripts/geometrytestblock.bs}
                    }
                }, {
                    type : Block,
                    props : {
                        Beat : 3653,
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
                }]
        }, {
            type : Lane,
            name : Fractal,
            Blocks : [{
                    type : FractalCubeBlock,
                    props : {
                        Beat : 4771,
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
                        Beat : 0,
                        Duration : 2048,
                        Name : DeferredLighting,
                        Color : [240, 179, 148],
                        _o_p_ : {
                            __schema__ : {
                                sizetypes : [],
                                names : [],
                                typeNames : []
                            }
                        },
                        script : {@RenderSystems/Lighting/DeferredPassBlock.bs}
                    }
                }, {
                    type : Block,
                    props : {
                        Beat : 2999,
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
            Blocks : [{
                    type : Block,
                    props : {
                        Beat : 0,
                        Duration : 2048,
                        Name : HdrPos,
                        Color : [240, 179, 148],
                        _o_p_ : {
                            __schema__ : {
                                sizetypes : [],
                                names : [],
                                typeNames : []
                            }
                        },
                        script : {@RenderSystems/Post/HdrPost.bs}
                    }
                }]
        }, {
            type : Lane,
            name : "",
            Blocks : [{
                    type : Block,
                    props : {
                        Beat : 0,
                        Duration : 2048,
                        Name : FinalCombine,
                        Color : [152, 144, 209],
                        _o_p_ : {
                            __schema__ : {
                                sizetypes : [],
                                names : [],
                                typeNames : []
                            }
                        },
                        script : {@RenderSystems/Post/FinalCombine.bs}
                    }
                }, {
                    type : Block,
                    props : {
                        Beat : 2311,
                        Duration : 1024,
                        Name : "",
                        Color : [128, 128, 128],
                        _o_p_ : {
                            __schema__ : {
                                sizetypes : [],
                                names : [],
                                typeNames : []
                            }
                        }
                    }
                }]
        }],
    properties : {
        Name : "",
        _o_p_ : {
            EnableTerrain : 0,
            Fov : 2.500000,
            CamPos : [-5.000000, 29.000000, -122.000000],
            CamDir : [0.000000, -0.100000, 1.000000],
            __schema__ : {
                sizetypes : [327692, 327692, 196612, 1],
                names : [CamPos, CamDir, Fov, EnableTerrain],
                typeNames : [float3, float3, float, bool]
            }
        }
    },
    master-script : {@Timeline/masterScript.bs}
}