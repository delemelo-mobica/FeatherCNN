//Tencent is pleased to support the open source community by making FeatherCNN available.

//Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.

//Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
//in compliance with the License. You may obtain a copy of the License at
//
//https://opensource.org/licenses/BSD-3-Clause
//
//Unless required by applicable law or agreed to in writing, software distributed
//under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//CONDITIONS OF ANY KIND, either express or implied. See the License for the
//specific language governing permissions and limitations under the License.

#pragma once

#include "../layer.h"

#include <math.h>
#include <float.h>

namespace feather
{
class SoftmaxLayer : public Layer
{
    public:
        SoftmaxLayer(RuntimeParameter<float>* rt_param)
            : Layer(rt_param)
        {
        }
        
        int Forward()
        {
            const Blob<float> *p_bottom = bottoms[0];
            const float *input = p_bottom->data();
            const size_t data_size = p_bottom->num() * p_bottom->channels() * p_bottom->height() * p_bottom->width();
            float *output = tops[0]->data();

            float sum = 0.0;
            float max = -FLT_MAX;
            for (size_t i = 0; i < data_size; ++i)
            {
                max = std::max<float>(max, input[i]);
            }
            for (size_t i = 0; i < data_size; ++i)
            {
                output[i] = static_cast<float>(exp(input[i] - max));
                sum += output[i];
            }
            for (size_t i = 0; i < data_size; ++i)
            {
                output[i] = output[i] / sum;
            }
            return 0;
        }
};
};
