################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
source/%.obj: ../source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1250/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="G:/UNIVERSITY FILES/MASTERS - ELECTRONIC AND DIGITAL SYSTEMS/3RD SEMESTER/Special Assignment/Final_Submission/Hercules-RM46L852-Bitflip_detector/CPU Compare Module(Lockstep Mode)_Bitflip-detector/include" --include_path="G:/UNIVERSITY FILES/MASTERS - ELECTRONIC AND DIGITAL SYSTEMS/3RD SEMESTER/Special Assignment/Final_Submission/Hercules-RM46L852-Bitflip_detector/CPU Compare Module(Lockstep Mode)_Bitflip-detector" --include_path="C:/ti/ccs1250/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/%.obj: ../source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1250/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="G:/UNIVERSITY FILES/MASTERS - ELECTRONIC AND DIGITAL SYSTEMS/3RD SEMESTER/Special Assignment/Final_Submission/Hercules-RM46L852-Bitflip_detector/CPU Compare Module(Lockstep Mode)_Bitflip-detector/include" --include_path="G:/UNIVERSITY FILES/MASTERS - ELECTRONIC AND DIGITAL SYSTEMS/3RD SEMESTER/Special Assignment/Final_Submission/Hercules-RM46L852-Bitflip_detector/CPU Compare Module(Lockstep Mode)_Bitflip-detector" --include_path="C:/ti/ccs1250/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


