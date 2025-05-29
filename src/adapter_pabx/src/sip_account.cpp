/**
 * @file adapter_pabx/sip_account.cpp
 * @brief Implementation of the SIPAccount class for SIP account registration and state management.
 */

#include <rclcpp/rclcpp.hpp>
#include "adapter_pabx/sip_account.hpp"

const rclcpp::Logger LOGGER = rclcpp::get_logger("sip_account");

bool SIPAccount::registerAccount(std::string username, std::string password, std::string domain) 
{
    username_ = username;
    password_ = password;
    domain_ = domain;

    try
    {
        account_config_.idUri = "sip:" + username_ + "@" + domain_;
        account_config_.regConfig.registrarUri = "sip:" + domain_;

        RCLCPP_INFO(LOGGER, "*** Registering account %s  ***", account_config_.idUri.c_str());

        auth_cred_info_ =  pj::AuthCredInfo("digest", "*", username_, 0, password_);
        account_config_.sipConfig.authCreds.push_back(auth_cred_info_);
        create(account_config_);
        return true;

    }
    catch(pj::Error &err)
    {
        std::string err_msg = "Failed to register account: " + err.info();
        RCLCPP_ERROR(LOGGER, err_msg.c_str());
        return false;
    }

}


std::string SIPAccount::getUsername() 
{
    return username_;
}

std::string SIPAccount::getDomain() 
{
    return domain_;
}


void SIPAccount::onRegState(pj::OnRegStateParam &prm [[maybe_unused]])
{
    pj::AccountInfo account_info = getInfo();
    if (!accountRegistered_ && account_info.regIsActive)
    {
        accountRegistered_ = true;
        RCLCPP_INFO(LOGGER, "*** Account %s@%s registered ***", username_.c_str(), domain_.c_str());
    }
    else if(accountRegistered_ && !account_info.regIsActive)
    {
        accountRegistered_ = false;
        RCLCPP_INFO(LOGGER, "*** Account %s@%s unregistered ***", username_.c_str(), domain_.c_str());
    }
    else
    {
        RCLCPP_WARN(LOGGER, "*** UNKOWN REGISTRATION STATE TRANSISTION for %s@%s", username_.c_str(), domain_.c_str());
        RCLCPP_WARN(LOGGER, "%d and %d", accountRegistered_, account_info.regIsActive);
    }
}