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

/*
 * For runtime parameters
 */

#pragma once

#include "blob.h"
#include "mempool.h"
#include "common.h"
#include <map>
// #define FEATHER_OPENCL
#ifdef FEATHER_OPENCL
#include "CLHPP/clhpp_runtime.hpp"
#endif

enum DeviceType
{
    CPU = 0,
    GPU_CL = 1,
    GPU_GL = 2
};

template<typename Dtype>
class RuntimeParameter
{
    public:
        RuntimeParameter() : _common_mempool(NULL),
            _device_type(DeviceType::CPU),
            _num_threads(1)
#ifdef FEATHER_OPENCL
            , _padded_input(NULL)
            , _padded_input_size(0)
#endif
        {
#ifdef FEATHER_OPENCL
            if (_device_type == DeviceType::GPU_CL)
            {
                _cl_runtime = new clhpp_feather::OpenCLRuntime();
            }
#endif
        }
        RuntimeParameter(CommonMemPool<Dtype> *common_mempool, DeviceType device_type, size_t num_threads)
            : _common_mempool(common_mempool),
              _num_threads(num_threads),
              _device_type(device_type)
#ifdef FEATHER_OPENCL
            , _padded_input(NULL)
            , _padded_input_size(0)
#endif
        {
#ifdef FEATHER_OPENCL
            if (_device_type == DeviceType::GPU_CL)
            {
                _cl_runtime = new clhpp_feather::OpenCLRuntime();
            }
#endif
        }
        ~RuntimeParameter()
        {
#ifdef FEATHER_OPENCL
            if (_device_type == DeviceType::GPU_CL)
            {
                delete _cl_runtime;
                _cl_runtime = NULL;

                if (_padded_input != NULL)
                {
                    delete _padded_input;
                    _padded_input = NULL;
                    _padded_input_size = 0;
                }
            }
#endif
        }

        CommonMemPool<Dtype> *common_mempool() const
        {
            return _common_mempool;
        }
        size_t num_threads() const
        {
            return _num_threads;
        }


        DeviceType device_type() const
        {
            return _device_type;
        }

#ifdef FEATHER_OPENCL
        cl::Context context() const
        {
            return _cl_runtime->context();
        }
        cl::CommandQueue command_queue() const
        {
            return _cl_runtime->command_queue();
        }
        cl::Device device() const
        {
            return _cl_runtime->device();
        }
        clhpp_feather::OpenCLMemType mem_type() const
        {
            return _cl_runtime->get_mem_type();
        }
        clhpp_feather::OpenCLRuntime* cl_runtime() const
        {
            return _cl_runtime;
        }
        size_t padded_input_size() const
        {
            return _padded_input_size;
        }
        feather::Blob<Dtype>* padded_input()
        {
            return _padded_input;
        }
        void update_padded_input_size(size_t size)
        {
            _padded_input_size = std::max(_padded_input_size, size);
        }
        void alloc_padded_input()
        {
            if (_padded_input == NULL && _padded_input_size)
            {
                _padded_input = new feather::Blob<Dtype>();
                _padded_input->AllocDevice(context(), _padded_input_size);
            }
        }
        void realloc_padded_input(size_t size)
        {
            if (size > _padded_input_size)
            {
                _padded_input_size = size;
                if (_padded_input != NULL)
                {
                    delete _padded_input;
                    _padded_input = NULL;
                }
                alloc_padded_input();
            }
        }
#endif

    private:
        CommonMemPool<Dtype> *_common_mempool;
        size_t _num_threads;
        DeviceType _device_type;

#ifdef FEATHER_OPENCL
        clhpp_feather::OpenCLRuntime *_cl_runtime;
        feather::Blob<Dtype>* _padded_input;
        size_t _padded_input_size;
#endif
};
