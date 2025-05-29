/**
 * @file adapter_pabx/announcement_server.cpp
 * @brief Implementation of the AnnouncementServer class for handling PABX announcement actions and SIP integration.
 */

#include <fstream>
#include "adapter_pabx/announcement_server.hpp"

const rclcpp::Logger LOGGER = rclcpp::get_logger("announcement_server");

AnnouncementServer::AnnouncementServer(std::string node_name)
    : rclcpp::Node(node_name)
{
    // Constructor implementation

    this->declare_parameter<std::string>("config_path", "/home/workspace/src/adapter_pabx/config/accounts.json");
    this->declare_parameter<int>("console_level", 4);

    loadAccountsFromConfig();

    initializeEndpoint();

    registerAccounts();

    startActionServer();

    RCLCPP_INFO(this->get_logger(), "Announcement Server initialized with %zu SIP accounts.", sip_accounts_.size());

    
}

AnnouncementServer::~AnnouncementServer()
{
    // Destructor implementation
    sip_endpoint_->stop();
    RCLCPP_INFO(this->get_logger(), "Announcement Server shut down successfully.");
}

void AnnouncementServer::loadAccountsFromConfig()
{
    config_path_ = this->get_parameter("config_path").as_string();
    if (config_path_.empty()) 
    {
        RCLCPP_ERROR(this->get_logger(), "Configuration file not specified. Please set the 'config_file' parameter.");
        return;
    }
    RCLCPP_INFO(this->get_logger(), "Using configuration file: %s", config_path_.c_str());

    // Load the accounts data from the configuration file
    std::ifstream config_file(config_path_);
    if (!config_file.is_open())
    {
        RCLCPP_ERROR(this->get_logger(), "Failed to open configuration file: %s", config_path_.c_str());
        return;
    }
    try {
        config_file >> accounts_data_;
        RCLCPP_INFO(this->get_logger(), "Loaded accounts data successfully.");
        RCLCPP_INFO(this->get_logger(), "Available accounts loaded from configuration file:");
        for (const auto& [account_name, _] : accounts_data_.items())
        {
            RCLCPP_INFO(this->get_logger(), "  - %s", account_name.c_str());
        }
    } catch (const nlohmann::json::parse_error &e) 
    {
        RCLCPP_ERROR(this->get_logger(), "Failed to parse configuration file: %s", e.what());
        return;
    }

    try
    {
        config_file.close();
        RCLCPP_INFO(this->get_logger(), "Configuration file closed successfully.");
    }
    catch (const std::exception &e)
    {
        RCLCPP_ERROR(this->get_logger(), "Failed to close configuration file: %s", e.what());
        return;
    }

}

void AnnouncementServer::initializeEndpoint()
{
    // Initialize the SIP endpoint
    try 
    {
        sip_endpoint_ = std::make_unique<SIPEndpoint>();
        sip_endpoint_->init(this->get_parameter("console_level").as_int());
        sip_endpoint_->start();
        RCLCPP_INFO(this->get_logger(), "SIP Endpoint initialized and started successfully.");
    } 
    catch (const std::exception &e) 
    {
        RCLCPP_ERROR(this->get_logger(), "Failed to initialize SIP Endpoint: %s", e.what());
        return;
    }
}

void AnnouncementServer::registerAccounts()
{
    // Register the SIP accounts from the configuration file
    for (const auto& [account_name, account_info] : accounts_data_.items()) {
        try {
            auto sip_account = std::make_unique<SIPAccount>();
            if (sip_account->registerAccount(
                    account_info["username"].get<std::string>(),
                    account_info["password"].get<std::string>(),
                    account_info["domain"].get<std::string>())) {
                sip_accounts_.push_back(std::move(sip_account));
                RCLCPP_INFO(this->get_logger(), "Registered SIP account: %s", account_name.c_str());
            } else {
                RCLCPP_ERROR(this->get_logger(), "Failed to register SIP account: %s", account_name.c_str());
            }
        } catch (const std::exception &e) {
            RCLCPP_ERROR(this->get_logger(), "Error registering SIP account %s: %s", account_name.c_str(), e.what());
        }
    }
}

void AnnouncementServer::startActionServer()
{

    auto handle_goal = [this](const rclcpp_action::GoalUUID & uuid [[maybe_unused]],
                        std::shared_ptr<const kabam_msgs::action::PabxAnnouncement::Goal> goal)
                    {
                        RCLCPP_INFO(this->get_logger(), "Call requested to extension: %d", goal->customer_extension);
                        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
                    };

    auto handle_cancel = [this](const std::shared_ptr<GoalHandleAnnouncementAction> goal_handle [[maybe_unused]])
                        {
                            RCLCPP_INFO(this->get_logger(), "Cancel requested for current call");
                            return rclcpp_action::CancelResponse::ACCEPT;
                        };

    auto handle_accepted = [this](const std::shared_ptr<GoalHandleAnnouncementAction> goal_handle)
                        {
                            RCLCPP_INFO(this->get_logger(), "Call accepted to extension: %d", goal_handle->get_goal()->customer_extension);
                            // TODO: Execute
                            executeAnnouncement(goal_handle);
                            return;
                        };


    // Create the action server
    announcement_action_server_ = rclcpp_action::create_server<AnnouncementAction>(
        this,
        "pabx_announcement",
        handle_goal,
        handle_cancel,
        handle_accepted);

    RCLCPP_INFO(this->get_logger(), "PABX Announcement Action Server started.");
}

void AnnouncementServer::executeAnnouncement(
    const std::shared_ptr<GoalHandleAnnouncementAction> goal_handle)
{
    // This function will handle the actual announcement logic
    RCLCPP_INFO(this->get_logger(), "Executing announcement for extension: %d", goal_handle->get_goal()->customer_extension);

    const auto goal = goal_handle->get_goal();
    std::string customer_extension = std::to_string(goal->customer_extension);
    std::string announcement_device_extension = std::to_string(goal->announcement_device_extension);
    std::string announcement_target = "sip:" + customer_extension + "@" + sip_accounts_[0]->getDomain();
    std::string transfer_target = "sip:" + announcement_device_extension + "@" + sip_accounts_[0]->getDomain();
    auto result = std::make_shared<kabam_msgs::action::PabxAnnouncement::Result>();

    // Configure call settings with audio only
    pj::CallOpParam prm(true);
    prm.opt.audioCount = 1;  // To disable audio stream count to 0
    prm.opt.videoCount = 0; // No video
    prm.opt.textCount = 0;  // No text stream

    try {
        sip_call_ = std::make_unique<SIPCall>(*sip_accounts_[0]);
        sip_call_->setGoalHandle(goal_handle);
        sip_call_->makeCall(announcement_target, transfer_target);
        RCLCPP_INFO(this->get_logger(), "Call initiated to %s", announcement_target.c_str());

        // Wait longer for call to complete (increased to 30 seconds)
        RCLCPP_INFO(LOGGER,"*** Waiting 5 seconds for call to connect ***" );  

        while(rclcpp::ok())
        {
   
            if (sip_call_->transferSuccess())
            {
                result->announcement_result = "announcement success";
                goal_handle->succeed(result);
                RCLCPP_INFO(LOGGER, "Playing announcement to %s", customer_extension.c_str());
                return;
            }
            else if (sip_call_->getStatus() == "DISCONNECTED" && !sip_call_->transferSuccess())
            {
                result->announcement_result = "announcement failed";
                goal_handle->abort(result);
                return;
            }

            sip_endpoint_->libHandleEvents(1000); // Handle events for 1 second
        }


    } catch (const std::exception &e) {
        RCLCPP_ERROR(this->get_logger(), "Failed to make call: %s", e.what());
        result->announcement_result = "Failed to make call";
        goal_handle->abort(result);
        return;
    }

    goal_handle->succeed(result);
    RCLCPP_INFO(this->get_logger(), "Announcement completed successfully.");
}

int main([[maybe_unused]] int argc , [[maybe_unused]] char **argv )
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<AnnouncementServer>("announcement_server");

    RCLCPP_INFO(node->get_logger(), "Announcement Server started successfully.");

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}