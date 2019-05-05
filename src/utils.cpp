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

#include "utils.h"
// #include "booster/helper.h"
#include <cstring>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

int ChkParamHeader(FILE* fp)
{
    fseek(fp, 0, SEEK_SET);
    int magic = 0;
    int nbr = fscanf(fp, "%d", &magic);
    if (nbr != 1)
    {
        fprintf(stderr, "issue with param file\n");
        return -1;
    }
    if (magic != 7767517)
    {
        fprintf(stderr, "param is too old, please regenerate\n");
        return -1;
    }
    return 0;
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

#if (defined(__linux__)  && !(defined(__aarch64__)))|| defined(__APPLE_CC__)
#else
void* _mm_malloc(size_t sz, size_t align)
{
    void *ptr;
#if (defined __APPLE__) || (defined _WIN32)
    return malloc(sz);
#else
    int alloc_result = posix_memalign(&ptr, align, sz);
    if (alloc_result != 0)
    {
        return NULL;
    }
    return ptr;
#endif
}

void _mm_free(void* ptr)
{
    if (NULL != ptr)
    {
        free(ptr);
        ptr = NULL;
    }
}
#endif

void StringTool::SplitString(const std::string &input, const std::string &delim, std::vector<std::string> &parts)
{
    for (char *s = strtok((char *)input.data(), (char *)delim.data()); s; s = strtok(NULL, (char *)delim.data()))
    {
        if (s != NULL)
        {
            parts.push_back(s);
        }
    }
}

void StringTool::RelaceString(std::string &input, const std::string &delim, const std::string& repstr)
{
    size_t pos = input.find(delim);
    while (pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        input.replace(pos, delim.size(), repstr);
        // Get the next occurrence from the current position
        pos = input.find(delim, pos + delim.size());
    }
}

#ifdef FEATHER_OPENCL
bool judge_android7_opencl()
{
    //libOpenCL.so
    //android7.0 sdk api 24
    char sdk[93] = "";
    __system_property_get("ro.build.version.sdk", sdk);
    if (std::atoi(sdk) < 24)
    {
        LOGI("[device] sdk [%d] < 24\n", std::atoi(sdk));
        return true;
    }

    bool flage = false;
    std::string lib_name1 = "libOpenCL.so";
    std::string lib_name2 = "libGLES_mali.so";
    std::vector<std::string> libraries_list =
    {
        "/vendor/etc/public.libraries.txt",
        "/system/etc/public.libraries.txt",
    };
    for (int i = 0; i < libraries_list.size(); i++)
    {
        std::ifstream out;
        std::string line;
        out.open(libraries_list[i].c_str());
        while (!out.eof())
        {
            std::getline(out, line);
            if (line.find(lib_name1) != line.npos || line.find(lib_name2) != line.npos)
            {
                LOGI("[public] %s:%s", libraries_list[i].c_str(), line.c_str());
                flage = true;
                break;
            }

        }
        out.close();
    }
    if(flage == false)
        return flage;

    flage = false;
    const std::vector<std::string> libpaths =
    {
        "libOpenCL.so",
#if defined(__aarch64__)
        // Qualcomm Adreno with Android
        "/system/vendor/lib64/libOpenCL.so",
        "/system/lib64/libOpenCL.so",
        // Mali with Android
        "/system/vendor/lib64/egl/libGLES_mali.so",
        "/system/lib64/egl/libGLES_mali.so",
        // Typical Linux board
        "/usr/lib/aarch64-linux-gnu/libOpenCL.so",
#else
        // Qualcomm Adreno with Android
        "/system/vendor/lib/libOpenCL.so",
        "/system/lib/libOpenCL.so",
        // Mali with Android
        "/system/vendor/lib/egl/libGLES_mali.so",
        "/system/lib/egl/libGLES_mali.so",
        // Typical Linux board
        "/usr/lib/arm-linux-gnueabihf/libOpenCL.so",
#endif
    };
    for (int i = 0; i < libpaths.size(); i++)
    {
        ifstream f(libpaths[i].c_str());
        if (f.good())
        {
            flage = true;
            LOGI("[libpaths]:%s", libpaths[i].c_str());
            break;
        }
    }
    return flage;
}
#endif

unsigned short hs_floatToHalf(float f)
{
    union
    {
        float d;
        unsigned int i;
    } u = { f };
    int s = (u.i >> 16) & 0x8000;
    int e = ((u.i >> 23) & 0xff) - 112;
    int m =          u.i & 0x7fffff;
    if (e <= 0)
    {
        if (e < -10) return s; /* underflowed */
        /* force leading 1 and round */
        m |= 0x800000;
        int t = 14 - e;
        int a = (1 << (t - 1)) - 1;
        int b = (m >> t) & 1;
        return s | ((m + a + b) >> t);
    }
    if (e == 143)
    {
        if (m == 0) return s | 0x7c00; /* +/- infinity */

        /* NaN, m == 0 forces us to set at least one bit and not become an infinity */
        m >>= 13;
        return s | 0x7c00 | m | (m == 0);
    }

    /* round the normalized float */
    m = m + 0xfff + ((m >> 13) & 1);

    /* significand overflow */
    if (m & 0x800000)
    {
        m =  0;
        e += 1;
    }

    /* exponent overflow */
    if (e > 30) return s | 0x7c00;

    return s | (e << 10) | (m >> 13);
}

int hs_halfToFloatRep(unsigned short c)
{
    int s = (c >> 15) & 0x001;
    int e = (c >> 10) & 0x01f;
    int m =         c & 0x3ff;
    if (e == 0)
    {
        if (m == 0) /* +/- 0 */ return s << 31;
        /* denormalized, renormalize it */
        while (!(m & 0x400))
        {
            m <<= 1;
            e -=  1;
        }
        e += 1;
        m &= ~0x400;
    }
    else if (e == 31) return (s << 31) | 0x7f800000 | (m << 13);   /* NaN or +/- infinity */
    e += 112;
    m <<= 13;
    return (s << 31) | (e << 23) | m;
}

float hs_halfToFloat(unsigned short c)
{
    union
    {
        float d;
        unsigned int i;
    } u;
    u.i = hs_halfToFloatRep(c);
    return u.d;
}
