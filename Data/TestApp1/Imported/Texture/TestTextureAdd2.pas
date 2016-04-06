{
    __type_guid__ : 1413824851,
    typename : Texture,
    class : Texture,
    inputs : [{
            class : AddOperator,
            inputs : [{
                    class : ConstantColorGenerator,
                    inputs : [],
                    props : {
                        Name : "",
                        Color : [132, 5, 212, 255]
                    }
                }, {
                    class : GradientGenerator,
                    inputs : [],
                    props : {
                        Name : "",
                        Color0 : [32, 32, 64, 255],
                        Color1 : [192, 32, 0, 255],
                        Point0 : [0.200000, 0.500000, 0.000000],
                        Point1 : [0.300000, 0.700000, 0.000000]
                    }
                }, {
                    class : GradientGenerator,
                    inputs : [],
                    props : {
                        Name : "",
                        Color0 : [32, 64, 32, 255],
                        Color1 : [0, 192, 128, 255],
                        Point0 : [0.700000, 0.700000, 0.000000],
                        Point1 : [0.500000, 0.900000, 0.000000]
                    }
                }],
            props : {
                Clamp : 0,
                Name : ""
            }
        }],
    props : {
        Name : Add2
    }
}