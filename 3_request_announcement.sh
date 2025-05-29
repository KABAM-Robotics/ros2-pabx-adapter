
docker exec adapter_pabx /ros_entrypoint.sh \
ros2 action send_goal \
/pabx_announcement kabam_msgs/action/PabxAnnouncement \
--feedback \
"{customer_extension: 501, announcement_device_extension: 101}"