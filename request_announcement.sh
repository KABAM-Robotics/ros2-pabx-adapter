
# Replace the <customer_extension> and <announcement_device_extension> with actual values

docker exec adapter_pabx /ros_entrypoint.sh \
ros2 action send_goal \
/pabx_announcement kabam_msgs/action/PabxAnnouncement \
--feedback \
"{customer_extension: <customer_extension>, announcement_device_extension: <announcement_device_extension>}"