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

#include "../feather_generated.h"
#include "../layer.h"
#include <CLHPP/opencl_kernels.hpp>

#include <math.h>
#include <limits>

namespace feather
{

template <class Dtype>
class PoolingLayerCL : public Layer<Dtype>
{
    public:
        PoolingLayerCL(const LayerParameter *layer_param, RuntimeParameter<Dtype>* rt_param);
        virtual int SetBuildOptions();
        virtual int SetKernelParameters();
        virtual int ForwardCL();
        virtual int ForwardReshapeCL();
        virtual int GenerateTopBlobs();
        inline void AssignOutputSize();


    private:
        bool fuse_relu;
        int input_height;
        int input_width;
        int input_channels;
        int output_height;
        int output_width;
        int output_channels;
        int pad_height;
        int pad_width;
        int kernel_height;
        int kernel_width;
        int stride_height;
        int stride_width;
        int channel_block_size;
        bool global_pooling;
        PoolingParameter_::PoolMethod method;
};
}; // namespace feather
