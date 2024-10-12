//===========================================================================
// Copyright (C) 2022 Intel Corporation
//
//
//
// SPDX-License-Identifier: MIT
//--------------------------------------------------------------------------

/**
 * @author daniel.enriquez.montanez@intel.com
 * @file  Sample_TelemetryAPP.cpp
 * @brief This file contains the 'main' function. Program execution begins and ends there.
 *
 */

#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <vector>

#include <iostream>
#include <string>

#include <windows.h>
#include <stdio.h>
#include "igcl_api.h"

#define PRINT_LOGS(...) printf(__VA_ARGS__)
#define LOG_AND_EXIT_ON_ERROR(Result, ErrRtrndByFunc)                       \
    if (CTL_RESULT_SUCCESS != Result)                                       \
    {                                                                       \
        printf("%s returned failure code: 0x%X\n", ErrRtrndByFunc, Result); \
        goto Exit;                                                          \
    }

std::string DecodeRetCode(ctl_result_t Res);
void CtlTemperatureTest(ctl_device_adapter_handle_t hDAhandle);
void CtlFrequencyTest(ctl_device_adapter_handle_t hDAhandle);
void CtlPowerTest(ctl_device_adapter_handle_t hDAhandle);
void CtlMemoryTest(ctl_device_adapter_handle_t hDAhandle);

void CtlTemperatureTest(ctl_device_adapter_handle_t hDAhandle)
{
    uint32_t TemperatureHandlerCount = 0;
    ctl_result_t res                 = ctlEnumTemperatureSensors(hDAhandle, &TemperatureHandlerCount, nullptr);
    if ((res != CTL_RESULT_SUCCESS) || TemperatureHandlerCount == 0)
    {
        PRINT_LOGS("\nTemperature component not supported. Error: %s", DecodeRetCode(res).c_str());
        return;
    }

    ctl_temp_handle_t* pTtemperatureHandle = new ctl_temp_handle_t[TemperatureHandlerCount];
    res = ctlEnumTemperatureSensors(hDAhandle, &TemperatureHandlerCount, pTtemperatureHandle);
    if (res != CTL_RESULT_SUCCESS)
    {
        PRINT_LOGS("\nError: %s for Temperature handle.", DecodeRetCode(res).c_str());
        goto cleanUp;
    }

    for (uint32_t i = 0; i < TemperatureHandlerCount; i++)
    {
        ctl_temp_properties_t temperatureProperties = { 0 };
        temperatureProperties.Size = sizeof(ctl_temp_properties_t);
        res = ctlTemperatureGetProperties(pTtemperatureHandle[i], &temperatureProperties);

        if (res != CTL_RESULT_SUCCESS)
        {
            PRINT_LOGS("\nError: %s from Temperature get properties.", DecodeRetCode(res).c_str());
        }
        else
        {
            if ((temperatureProperties.type == CTL_TEMP_SENSORS_GPU))
            {
                PRINT_LOGS("\n[HotSpotTemperature]: %u", (uint32_t)temperatureProperties.maxTemperature);

                double temperature = 0;
                res = ctlTemperatureGetState(pTtemperatureHandle[i], &temperature);

                if (res != CTL_RESULT_SUCCESS)
                {
                    PRINT_LOGS("\nError: %s  from Temperature get state.", DecodeRetCode(res).c_str());
                }
                else
                {
                    PRINT_LOGS("\n[CoreTemperature]: %f", temperature);
                }
            }
        }
    }

cleanUp:
    delete[] pTtemperatureHandle;
    pTtemperatureHandle = nullptr;
}

void CtlFrequencyTest(ctl_device_adapter_handle_t hDAhandle)
{

    uint32_t FrequencyHandlerCount = 0;
    ctl_result_t res = ctlEnumFrequencyDomains(hDAhandle, &FrequencyHandlerCount, nullptr);
    if ((res != CTL_RESULT_SUCCESS) || FrequencyHandlerCount == 0)
    {
        PRINT_LOGS("\nTemperature component not supported. Error: %s", DecodeRetCode(res).c_str());
        return;
    }

    ctl_freq_handle_t* pFrequencyHandle = new ctl_freq_handle_t[FrequencyHandlerCount];

    res = ctlEnumFrequencyDomains(hDAhandle, &FrequencyHandlerCount, pFrequencyHandle);

    if (res != CTL_RESULT_SUCCESS)
    {
        PRINT_LOGS("\nError: %s for Frequency handle.", DecodeRetCode(res).c_str());
        goto cleanUp;
    }

    for (uint32_t i = 0; i < FrequencyHandlerCount; i++)
    {
        ctl_freq_properties_t freqProperties = { 0 };
        freqProperties.Size = sizeof(ctl_freq_properties_t);
        res = ctlFrequencyGetProperties(pFrequencyHandle[i], &freqProperties);
        if (res)
        {
            PRINT_LOGS("\n from Frequency get properties. %s", DecodeRetCode(res).c_str());
        }

        ctl_freq_state_t freqState = { 0 };
        freqState.Size = sizeof(ctl_freq_state_t);
        res = ctlFrequencyGetState(pFrequencyHandle[i], &freqState);
        if (res)
        {
            PRINT_LOGS("\n %s from Frequency get state.", DecodeRetCode(res).c_str());
        }
        else
        {
            if (freqProperties.type == CTL_FREQ_DOMAIN_GPU) 
            {
                PRINT_LOGS("\n[CoreFrequency]: %f Mhz\n", freqState.actual);
            }
            if (freqProperties.type == CTL_FREQ_DOMAIN_MEMORY) 
            {
                PRINT_LOGS("\n[MemoryFrequency] %f Mhz\n", freqState.actual);
            }
        }
    }

cleanUp:
    delete[] pFrequencyHandle;
    pFrequencyHandle = nullptr;
}

void CtlPowerTest(ctl_device_adapter_handle_t hDAhandle)
{
    uint32_t PowerHandlerCount = 0;
    ctl_result_t res           = ctlEnumPowerDomains(hDAhandle, &PowerHandlerCount, nullptr);
    if ((res != CTL_RESULT_SUCCESS) || PowerHandlerCount == 0)
    {
        PRINT_LOGS("\nPower component not supported. Error: %s", DecodeRetCode(res).c_str());
        return;
    }
    else
    {
        //PRINT_LOGS("\nNumber of Power Handles [%u]", PowerHandlerCount);
    }

    ctl_pwr_handle_t *pPowerHandle = new ctl_pwr_handle_t[PowerHandlerCount];

    res = ctlEnumPowerDomains(hDAhandle, &PowerHandlerCount, pPowerHandle);

    if (res != CTL_RESULT_SUCCESS)
    {
        PRINT_LOGS("\nError: %s for Power handle.", DecodeRetCode(res).c_str());
        goto cleanUp;
    }

    for (uint32_t i = 0; i < PowerHandlerCount; i++)
    {
        ctl_power_properties_t properties = { 0 };
        properties.Size                   = sizeof(ctl_power_properties_t);
        res                               = ctlPowerGetProperties(pPowerHandle[i], &properties);


        if (res != CTL_RESULT_SUCCESS)
        {
            PRINT_LOGS("\n %s from Power get properties.", DecodeRetCode(res).c_str());
        }


        ctl_power_energy_counter_t energyCounter = { 0 };
        energyCounter.Size                       = sizeof(ctl_power_energy_counter_t);
        res                                      = ctlPowerGetEnergyCounter(pPowerHandle[i], &energyCounter);

        if (res != CTL_RESULT_SUCCESS)
        {
            PRINT_LOGS("\n %s from Power get energy counter.", DecodeRetCode(res).c_str());
        }
        else
        {
            //PRINT_LOGS("\n[Power] Energy Counter [%llu] micro J", energyCounter.energy);
            //PRINT_LOGS("\n[Power] Time Stamp [%llu] time stamp", energyCounter.timestamp);
        }

        // 等待一段时间，确保能量消耗有明显差异（如1秒钟）
        Sleep(200);

        // 第二次获取能量计数 (s2)
        ctl_power_energy_counter_t energyCounter2 = { 0 };
        energyCounter2.Size = sizeof(ctl_power_energy_counter_t);
        res = ctlPowerGetEnergyCounter(pPowerHandle[i], &energyCounter2);

        if (res != CTL_RESULT_SUCCESS)
        {
            PRINT_LOGS("\n %s from Power get energy counter.", DecodeRetCode(res).c_str());
        }
        else
        {
            //PRINT_LOGS("\n[Power] Energy Counter 2 [%llu] micro J", energyCounter2.energy);
            //PRINT_LOGS("\n[Power] Time Stamp 2 [%llu] time stamp", energyCounter2.timestamp);
        }

        // 计算能量差值和时间差值
        uint64_t energyDelta = energyCounter2.energy - energyCounter.energy;  // 单位为微焦耳 (micro J)
        uint64_t timeDelta = energyCounter2.timestamp - energyCounter.timestamp;  // 单位为微秒 (microseconds)

        // 将微焦耳转换为焦耳，微秒转换为秒
        double energyJoules = energyDelta / 1e6;  // 转换为焦耳 (J)
        double timeSeconds = timeDelta / 1e6;    // 转换为秒 (s)

        // 计算平均功率 (W)
        double powerWatts = energyJoules / timeSeconds;

        PRINT_LOGS("\n[PackagePower] PackagePower: %.6f Watts", powerWatts);
    }

cleanUp:
    delete[] pPowerHandle;
    pPowerHandle = nullptr;
}

void CtlMemoryTest(ctl_device_adapter_handle_t hDAhandle)
{

    uint32_t MemoryHandlerCount = 0;
    ctl_result_t res = ctlEnumMemoryModules(hDAhandle, &MemoryHandlerCount, nullptr);
    if ((res != CTL_RESULT_SUCCESS) || MemoryHandlerCount == 0)
    {
        PRINT_LOGS("\nMemory component not supported. Error: %s", DecodeRetCode(res).c_str());
        return;
    }
    ctl_mem_handle_t* pMemoryHandle = new ctl_mem_handle_t[MemoryHandlerCount];

    res = ctlEnumMemoryModules(hDAhandle, &MemoryHandlerCount, pMemoryHandle);

    if (res != CTL_RESULT_SUCCESS)
    {
        PRINT_LOGS("\nError: %s for Memory handle.", DecodeRetCode(res).c_str());
        goto cleanUp;
    }

    ctl_mem_properties_t memoryProperties = { 0 };
    memoryProperties.Size = sizeof(ctl_mem_properties_t);
    res = ctlMemoryGetProperties(pMemoryHandle[0], &memoryProperties);

    if (res != CTL_RESULT_SUCCESS)
    {
        PRINT_LOGS("\nError: %s from Memory get properties.", DecodeRetCode(res).c_str());
    }
    ctl_mem_state_t state = { 0 };
    state.Size = sizeof(ctl_mem_state_t);
    res = ctlMemoryGetState(pMemoryHandle[0], &state);

    if (res != CTL_RESULT_SUCCESS)
    {
        PRINT_LOGS("\nError: %s from Memory get State.", DecodeRetCode(res).c_str());
    }
    else
    {
        PRINT_LOGS("\n[MemoryUsage]: %f", (state.size - state.free) * 1.0f / state.size * 100.0f);
        PRINT_LOGS("\n[MemoryTotal]: %f MB", state.size / 1024.0f / 1024.0f);
        PRINT_LOGS("\n[MemoryFree]: %f MB", state.free / 1024.0f / 1024.0f);
        PRINT_LOGS("\n[MemoryUsed]: %f MB", (state.size - state.free) / 1024.0f / 1024.0f);
    }
cleanUp:
    delete[] pMemoryHandle;
    pMemoryHandle = nullptr;
}

void CtlEngineTest(ctl_device_adapter_handle_t hDAhandle)
{
    uint32_t EngineHandlerCount = 0;
    ctl_result_t res = ctlEnumEngineGroups(hDAhandle, &EngineHandlerCount, nullptr);
    if ((res != CTL_RESULT_SUCCESS) || EngineHandlerCount == 0)
    {
        PRINT_LOGS("\nEngine component not supported. Error: %s", DecodeRetCode(res).c_str());
        return;
    }

    ctl_engine_handle_t* pEngineHandle = new ctl_engine_handle_t[EngineHandlerCount];

    res = ctlEnumEngineGroups(hDAhandle, &EngineHandlerCount, pEngineHandle);

    if (res != CTL_RESULT_SUCCESS)
    {
        PRINT_LOGS("\nError: %s for Engine handle.", DecodeRetCode(res).c_str());
        goto cleanUp;
    }

    for (uint32_t i = 0; i < EngineHandlerCount; i++)
    {
        ctl_engine_properties_t engineProperties = { 0 };
        engineProperties.Size = sizeof(ctl_engine_properties_t);
        res = ctlEngineGetProperties(pEngineHandle[i], &engineProperties);
        if (res)
        {
            PRINT_LOGS("\nError: %s from Engine get properties.", DecodeRetCode(res).c_str());
        }
        else
        {
            if ((engineProperties.type == CTL_ENGINE_GROUP_RENDER))
            {
                ctl_engine_stats_t engineStats = { 0 };
                engineStats.Size = sizeof(ctl_engine_stats_t);
                int32_t iterations = 3;
                uint64_t prevActiveCounter = 0, prevTimeStamp = 0;
                auto ActivityRes = ctlEngineGetActivity(pEngineHandle[i], &engineStats);
                prevActiveCounter = engineStats.activeTime;
                prevTimeStamp = engineStats.timestamp;
                Sleep(200);

                ActivityRes = ctlEngineGetActivity(pEngineHandle[i], &engineStats);

                uint64_t activeDiff = engineStats.activeTime - prevActiveCounter;
                uint64_t timeWindow = engineStats.timestamp - prevTimeStamp;

                double percentActivity = static_cast<double>(activeDiff) / static_cast<double>(timeWindow);
                percentActivity *= 100.0;

                PRINT_LOGS("[CoreUsage] %f\n", percentActivity);
            }
        }
    }

cleanUp:
    delete[] pEngineHandle;
    pEngineHandle = nullptr;
}

std::string DecodeRetCode(ctl_result_t Res)
{
    switch (Res)
    {
    case CTL_RESULT_SUCCESS:
    {
        return std::string("[CTL_RESULT_SUCCESS]");
    }
    case CTL_RESULT_ERROR_CORE_OVERCLOCK_NOT_SUPPORTED:
    {
        return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_NOT_SUPPORTED]");
    }
    case CTL_RESULT_ERROR_CORE_OVERCLOCK_VOLTAGE_OUTSIDE_RANGE:
    {
        return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_VOLTAGE_OUTSIDE_RANGE]");
    }
    case CTL_RESULT_ERROR_CORE_OVERCLOCK_FREQUENCY_OUTSIDE_RANGE:
    {
        return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_FREQUENCY_OUTSIDE_RANGE]");
    }
    case CTL_RESULT_ERROR_CORE_OVERCLOCK_POWER_OUTSIDE_RANGE:
    {
        return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_POWER_OUTSIDE_RANGE]");
    }
    case CTL_RESULT_ERROR_CORE_OVERCLOCK_TEMPERATURE_OUTSIDE_RANGE:
    {
        return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_TEMPERATURE_OUTSIDE_RANGE]");
    }
    case CTL_RESULT_ERROR_GENERIC_START:
    {
        return std::string("[CTL_RESULT_ERROR_GENERIC_START]");
    }
    case CTL_RESULT_ERROR_CORE_OVERCLOCK_RESET_REQUIRED:
    {
        return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_RESET_REQUIRED]");
    }
    case CTL_RESULT_ERROR_CORE_OVERCLOCK_IN_VOLTAGE_LOCKED_MODE:
    {
        return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_IN_VOLTAGE_LOCKED_MODE");
    }
    case CTL_RESULT_ERROR_CORE_OVERCLOCK_WAIVER_NOT_SET:
    {
        return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_WAIVER_NOT_SET]");
    }
    case CTL_RESULT_ERROR_NOT_INITIALIZED:
    {
        return std::string("[CTL_RESULT_ERROR_NOT_INITIALIZED]");
    }
    case CTL_RESULT_ERROR_ALREADY_INITIALIZED:
    {
        return std::string("[CTL_RESULT_ERROR_ALREADY_INITIALIZED]");
    }
    case CTL_RESULT_ERROR_DEVICE_LOST:
    {
        return std::string("[CTL_RESULT_ERROR_DEVICE_LOST]");
    }
    case CTL_RESULT_ERROR_INSUFFICIENT_PERMISSIONS:
    {
        return std::string("[CTL_RESULT_ERROR_INSUFFICIENT_PERMISSIONS]");
    }
    case CTL_RESULT_ERROR_NOT_AVAILABLE:
    {
        return std::string("[CTL_RESULT_ERROR_NOT_AVAILABLE]");
    }
    case CTL_RESULT_ERROR_UNINITIALIZED:
    {
        return std::string("[CTL_RESULT_ERROR_UNINITIALIZED]");
    }
    case CTL_RESULT_ERROR_UNSUPPORTED_VERSION:
    {
        return std::string("[CTL_RESULT_ERROR_UNSUPPORTED_VERSION]");
    }
    case CTL_RESULT_ERROR_UNSUPPORTED_FEATURE:
    {
        return std::string("[CTL_RESULT_ERROR_UNSUPPORTED_FEATURE]");
    }
    case CTL_RESULT_ERROR_INVALID_ARGUMENT:
    {
        return std::string("[CTL_RESULT_ERROR_INVALID_ARGUMENT]");
    }
    case CTL_RESULT_ERROR_INVALID_NULL_HANDLE:
    {
        return std::string("[CTL_RESULT_ERROR_INVALID_NULL_HANDLE]");
    }
    case CTL_RESULT_ERROR_INVALID_NULL_POINTER:
    {
        return std::string("[CTL_RESULT_ERROR_INVALID_NULL_POINTER]");
    }
    case CTL_RESULT_ERROR_INVALID_SIZE:
    {
        return std::string("[CTL_RESULT_ERROR_INVALID_SIZE]");
    }
    case CTL_RESULT_ERROR_UNSUPPORTED_SIZE:
    {
        return std::string("[CTL_RESULT_ERROR_UNSUPPORTED_SIZE]");
    }
    case CTL_RESULT_ERROR_NOT_IMPLEMENTED:
    {
        return std::string("[CTL_RESULT_ERROR_NOT_IMPLEMENTED]");
    }
    case CTL_RESULT_ERROR_ZE_LOADER:
    {
        return std::string("[CTL_RESULT_ERROR_ZE_LOADER]");
    }
    case CTL_RESULT_ERROR_INVALID_OPERATION_TYPE:
    {
        return std::string("[CTL_RESULT_ERROR_INVALID_OPERATION_TYPE]");
    }
    case CTL_RESULT_ERROR_UNKNOWN:
    {
        return std::string("[CTL_RESULT_ERROR_UNKNOWN]");
    }
    default:
        return std::string("[Unknown Error]");
    }
}

void PerComponentTest(ctl_device_adapter_handle_t hDAhandle)
{
    //PackagePower
    CtlPowerTest(hDAhandle);

    //CoreTemperature HotSpotTemperature
    CtlTemperatureTest(hDAhandle);

    //CoreFrequency 
    //MemoryFrequency
    CtlFrequencyTest(hDAhandle);

    //CoreUsage
    CtlEngineTest(hDAhandle);

    //MemoryUsage 
    //MemoryUsed
    //MemoryFree
    //MemoryTotal
    //D3DDedicatedMemoryUsed
    //D3DSharedMemoryUsed
    CtlMemoryTest(hDAhandle);
}

int main()
{
    ctl_result_t Result = CTL_RESULT_SUCCESS;
    ctl_device_adapter_handle_t* hDevices = nullptr;
    ctl_device_adapter_properties_t StDeviceAdapterProperties = { 0 };
    // Get a handle to the DLL module.
    uint32_t Adapter_count = 0;
    uint32_t Display_count = 0;
    uint32_t Index = 0;
    uint32_t Display_index = 0;

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    ctl_init_args_t CtlInitArgs;
    ctl_api_handle_t hAPIHandle;
    CtlInitArgs.AppVersion = CTL_MAKE_VERSION(CTL_IMPL_MAJOR_VERSION, CTL_IMPL_MINOR_VERSION);
    CtlInitArgs.flags = CTL_INIT_FLAG_USE_LEVEL_ZERO;
    CtlInitArgs.Size = sizeof(CtlInitArgs);
    CtlInitArgs.Version = 0;
    ZeroMemory(&CtlInitArgs.ApplicationUID, sizeof(ctl_application_id_t));
    try
    {
        Result = ctlInit(&CtlInitArgs, &hAPIHandle);
        LOG_AND_EXIT_ON_ERROR(Result, "ctlInit");
    }
    catch (const std::bad_array_new_length& e)
    {
        printf("%s \n", e.what());
    }

    if (CTL_RESULT_SUCCESS == Result)
    {
        try
        {
            Result = ctlEnumerateDevices(hAPIHandle, &Adapter_count, hDevices);
            LOG_AND_EXIT_ON_ERROR(Result, "ctlEnumerateDevices");
        }
        catch (const std::bad_array_new_length& e)
        {
            printf("%s \n", e.what());
        }

        if (CTL_RESULT_SUCCESS == Result)
        {
            hDevices = (ctl_device_adapter_handle_t*)malloc(sizeof(ctl_device_adapter_handle_t) * Adapter_count);
            if (hDevices == NULL)
            {
                return ERROR;
            }
            try
            {
                Result = ctlEnumerateDevices(hAPIHandle, &Adapter_count, hDevices);
                LOG_AND_EXIT_ON_ERROR(Result, "ctlEnumerateDevices");
            }
            catch (const std::bad_array_new_length& e)
            {
                printf("%s \n", e.what());
            }
        }
        if (CTL_RESULT_SUCCESS != Result)
        {
            printf("ctlEnumerateDevices returned failure code: 0x%X\n", Result);
            goto Exit;
        }

        for (Index = 0; Index < Adapter_count; Index++)
        {
            if (NULL != hDevices[Index])
            {
                LUID AdapterID;
                StDeviceAdapterProperties.Size = sizeof(ctl_device_adapter_properties_t);
                StDeviceAdapterProperties.pDeviceID = malloc(sizeof(LUID));
                StDeviceAdapterProperties.device_id_size = sizeof(LUID);
                StDeviceAdapterProperties.Version = 2;

                if (NULL == StDeviceAdapterProperties.pDeviceID)
                {
                    return ERROR;
                }

                Result = ctlGetDeviceProperties(hDevices[Index], &StDeviceAdapterProperties);

                if (Result != CTL_RESULT_SUCCESS)
                {
                    printf("ctlGetDeviceProperties returned failure code: 0x%X\n", Result);
                    break;
                }

                if (CTL_DEVICE_TYPE_GRAPHICS != StDeviceAdapterProperties.device_type)
                {
                    printf("This is not a Graphics device \n");

                    if (NULL != StDeviceAdapterProperties.pDeviceID)
                    {
                        free(StDeviceAdapterProperties.pDeviceID);
                    }
                    continue;
                }

                // Per Component Tests
                PerComponentTest(hDevices[Index]);
            }
        }
        Sleep(50000);
    Exit:

        ctlClose(hAPIHandle);

        if (hDevices != nullptr)
        {
            free(hDevices);
            hDevices = nullptr;
        }
        Sleep(50000);
        return 0;
    }
}