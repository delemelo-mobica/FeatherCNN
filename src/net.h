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

#include "layer.h"
#include "rt_param.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <sstream>

namespace feather
{
class Net
{
    public:
        Net();

        ~Net();

        int LoadParam(const char * param_path);
        int LoadParam(FILE* fp);
        int LoadWeights(const char * weights_path);
        int LoadWeights(FILE* fp);
        
        void SetInput(const char* input_name, const void * input_data);
        int Forward();
          
        int GetBlobDataSize(size_t *data_size, std::string blob_name);
        int PrintBlobData(std::string blob_name);
        int ExtractBlob(float *output_ptr, std::string blob_name);

        std::map<std::string, Blob<float> *> blob_map;
    private:
        RuntimeParameter<float> *rt_param;
        std::vector<Layer *> layers;
};
}; // namespace feather
