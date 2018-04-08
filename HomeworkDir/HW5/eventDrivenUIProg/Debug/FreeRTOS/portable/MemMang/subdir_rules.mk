################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/portable/MemMang/heap_2.obj: ../FreeRTOS/portable/MemMang/heap_2.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Users/pavandhareshwar/CCS/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/pavandhareshwar/Downloads/TivaWare/SW-TM4C-2.1.4.178/inc" --include_path="/Users/pavandhareshwar/Downloads/TivaWare/SW-TM4C-2.1.4.178" --include_path="/Users/pavandhareshwar/workspace_v8/freeRTOSTest3Tasks" --include_path="/Users/pavandhareshwar/workspace_v8/freeRTOSTest3Tasks/FreeRTOS/include" --include_path="/Users/pavandhareshwar/workspace_v8/freeRTOSTest3Tasks/FreeRTOS/portable/CCS/ARM_CM4F" --include_path="/Users/pavandhareshwar/CCS/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS/portable/MemMang/heap_2.d_raw" --obj_directory="FreeRTOS/portable/MemMang" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


