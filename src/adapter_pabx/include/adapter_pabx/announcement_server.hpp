#ifndef ADAPTER_PABX_ANNOUNCEMENT_SERVER_HPP
#define ADAPTER_PABX_ANNOUNCEMENT_SERVER_HPP

/***
 * @file adapter_pabx/announcement_server.hpp
 * @brief AnnouncementServer class that provides the ROS2 action server to handle PABX announcements.
 * @author V Mohammed Ibrahim (ibrahim.mohammed@kabam.ai)
 * @date 2025-05-26
 */


#include <nlohmann/json.hpp>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <kabam_msgs/action/pabx_announcement.hpp>

#include "adapter_pabx/sip_endpoint.hpp"
#include "adapter_pabx/sip_account.hpp"
#include "adapter_pabx/sip_call.hpp"

using AnnouncementAction = kabam_msgs::action::PabxAnnouncement;
using GoalHandleAnnouncementAction = rclcpp_action::ServerGoalHandle<AnnouncementAction>;

class AnnouncementServer: public rclcpp::Node
{
public:

    /**
     * @brief Constructor for the AnnouncementServer class.
     */
    AnnouncementServer(std::string node_name = "announcement_server");

    /**
     * @brief Destructor for the AnnouncementServer class.
     */
    ~AnnouncementServer();

private:

    // SIP endpoint for handling SIP communications
    std::unique_ptr<SIPEndpoint> sip_endpoint_;

    // SIP accounts available for making PABX announcements
    std::vector<std::unique_ptr<SIPAccount>> sip_accounts_;

    // SIP call object 
    std::unique_ptr<SIPCall> sip_call_;

    // Configuration file containing the caller account details
    std::string config_path_;

    // JSON object to hold the SIP caller accounts available for PABX announcements
    nlohmann::json accounts_data_;

    // ROS2 action server for handling PABX announcements
    rclcpp_action::Server<AnnouncementAction>::SharedPtr announcement_action_server_;

    /**
     * @brief Load SIP accounts from the configuration file.
     *
     * Reads the JSON configuration file specified by the config_path_ parameter and loads SIP account credentials.
     */
    void loadAccountsFromConfig();

    /**
     * @brief Initialize the SIP endpoint for handling SIP communications.
     *
     * Creates and configures the SIPEndpoint instance for SIP signaling and media.
     */
    void initializeEndpoint();

    /**
     * @brief Register the SIP accounts from the configuration file.
     *
     * Iterates through loaded account data and registers each SIP account with the endpoint.
     */
    void registerAccounts();

    /**
     * @brief Start the ROS2 action server for PABX announcements.
     */
    void startActionServer();

    /**
     * @brief Execute the announcement action logic for a given goal handle.
     * @param goal_handle The action goal handle for the announcement request.
     */
    void executeAnnouncement(
        const std::shared_ptr<GoalHandleAnnouncementAction> goal_handle);

};

#endif // ADAPTER_PABX_ANNOUNCEMENT_SERVER_HPP