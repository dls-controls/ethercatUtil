TOP=../..
include $(TOP)/configure/CONFIG

#----------------------------------------
#  Template files
#----------------------------------------

# Generic module templates
DB += ethercat_gui_analog_input_module.template
DB += ethercat_gui_analog_output_module.template
DB += ethercat_gui_digital_input_module.template
DB += ethercat_gui_digital_output_module.template
DB += ethercat_gui_power_supply_module.template

# Generic channel templates
DB += ethercat_gui_analog_output_channel.template
DB += ethercat_gui_digital_output_channel.template
DB += ethercat_gui_input_channel.template

# Custom module templates
DB += ethercat_gui_ELM3704_module.template

# Custom channel templates
DB += ethercat_gui_ELM3704_channel.template

# GUI templates
DB += ethercat_gui_ELM3704_channel_detail.template

# Utility templates
DB += input_channel_scale_offset_sync.template
DB += input_channel_scale_offset_sync_channel.template

#----------------------------------------

include $(TOP)/configure/RULES
