/**
 * @file adapter_pabx/sip_endpoint.cpp
 * @brief Implementation of the SIPEndpoint class for managing SIP endpoint lifecycle and configuration.
 */

#include <rclcpp/rclcpp.hpp>
#include "adapter_pabx/sip_endpoint.hpp"


const rclcpp::Logger LOGGER = rclcpp::get_logger("sip_endpoint");

SIPEndpoint::SIPEndpoint()
{
    RCLCPP_INFO(LOGGER, "Creating Endpoint");

    this->libCreate();

    RCLCPP_INFO(LOGGER, "SIP Endpoint created");
}

SIPEndpoint::~SIPEndpoint()
{
    RCLCPP_INFO(LOGGER, "Destroying Endpoint");
    try
    {
        this->libDestroy();
        RCLCPP_INFO(LOGGER, "SIP Endpoint destroyed");
    }
    catch (const pj::Error &err)
    {
        RCLCPP_ERROR(LOGGER, "Failed to destroy SIP Endpoint: %s", err.info().c_str());
    }
}   

void SIPEndpoint::init(int console_level)
{
     RCLCPP_INFO(LOGGER, "Configuring Endpoint");
    // Set logging configuration
    endpoint_config_.logConfig.level = 5; // Set log level to 5 (debug)
    endpoint_config_.logConfig.consoleLevel = console_level; // Set console log level to 4 (info)

    // Set media configuration
    endpoint_config_.medConfig.maxMediaPorts = 16;
    endpoint_config_.medConfig.channelCount = 2;

    RCLCPP_INFO(LOGGER, "Initialzing Endpoint");
    this->libInit(endpoint_config_);

    RCLCPP_INFO(LOGGER, "Configuring transport");
    
    transport_config_.port = 5060; // Default SIP port
    transport_config_.qosType = PJ_QOS_TYPE_BEST_EFFORT;
    transport_config_.boundAddress = "";
    transport_config_.publicAddress = "";

    RCLCPP_INFO(LOGGER, "Creating transport");
    try
    {
        this->transportCreate(PJSIP_TRANSPORT_UDP, transport_config_);
        RCLCPP_INFO(LOGGER, "Transport created successfully");
    }
    catch (const pj::Error &err)
    {
        RCLCPP_ERROR(LOGGER, "Failed to create transport: %s", err.info().c_str());
        throw std::runtime_error("Failed to create SIP transport");
    }
   
}


void SIPEndpoint::start()
{
    RCLCPP_INFO(LOGGER, "Starting Endpoint");
    try
    {
        this->libStart();
        RCLCPP_INFO(LOGGER, "SIP Endpoint started");
    }
    catch (const pj::Error &err)
    {
        RCLCPP_ERROR(LOGGER, "Failed to start SIP Endpoint: %s", err.info().c_str());
        throw std::runtime_error("Failed to start SIP Endpoint");
    }

    RCLCPP_INFO(LOGGER, "Setting audio device to null");
    // Set audio device to null to avoid audio playback
    this->audDevManager().setNullDev();
}


void SIPEndpoint::stop()
{
    RCLCPP_INFO(LOGGER, "Shutting down Endpoint");
    try
    {
        this->libDestroy();
        RCLCPP_INFO(LOGGER, "SIP Endpoint shutdown successfully");
    }
    catch (const pj::Error &err)
    {
        RCLCPP_ERROR(LOGGER, "Failed to shutdown SIP Endpoint: %s", err.info().c_str());
    }
}