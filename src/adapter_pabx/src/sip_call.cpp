/**
 * @file adapter_pabx/sip_call.cpp
 * @brief Implementation of the SIPCall class for managing SIP call operations and state transitions.
 */

#include <rclcpp/rclcpp.hpp>
#include "adapter_pabx/sip_call.hpp"

const rclcpp::Logger LOGGER = rclcpp::get_logger("sip_call");

SIPCall::SIPCall(pj::Account &account, int call_id)
    : pj::Call(account, call_id)
{
    // Constructor implementation
}

SIPCall::~SIPCall()
{
    // Destructor implementation
}

void SIPCall::makeCall(const std::string &dest_uri, std::string transfer_uri)
{
    transfer_uri_ = transfer_uri;
    pj::CallOpParam call_param(true);
    pj::Call::makeCall(dest_uri, call_param);
    RCLCPP_INFO(LOGGER, "Initiated call to %s", dest_uri.c_str());
}

void SIPCall::onCallState(pj::OnCallStateParam &prm [[maybe_unused]])
{
    pj::CallInfo call_info;
    try {
        call_info = getInfo();
    } catch (const pj::Error& err) {
        RCLCPP_ERROR(LOGGER, "onCallState: %s", err.info().c_str());
        return;
    }
    RCLCPP_INFO(LOGGER, "*** Call state %s ***", call_info.stateText.c_str());

    switch (call_info.state) {
        case pjsip_inv_state::PJSIP_INV_STATE_EARLY:
            call_status_ = "RINGING";
            RCLCPP_INFO(LOGGER, "*** Call ringing ***");
            break;
        case pjsip_inv_state::PJSIP_INV_STATE_CONFIRMED:
            call_status_ = "CONNECTED";
            RCLCPP_INFO(LOGGER, "*** Call connected  ***");
            // Transfer after connection
            std::this_thread::sleep_for(std::chrono::seconds(2));
            RCLCPP_INFO(LOGGER, "*** Transfering call ***");
            {
                pj::CallOpParam param;
                xfer(transfer_uri_, param);
            }
            break;
        case pjsip_inv_state::PJSIP_INV_STATE_DISCONNECTED:
            call_status_ = "DISCONNECTED";
            RCLCPP_INFO(LOGGER, "*** Call disconnected: %d %s ***", call_info.lastStatusCode, call_info.lastReason.c_str());
            break;
        default:
            call_status_ = call_info.stateText;
            // Optionally, you could capitalize call_info.stateText here if needed
            break;
    }

    // Publish feedback if goal_handle is valid
    auto goal_handle = goal_handle_.lock();
    if (goal_handle) {
        auto feedback = std::make_shared<kabam_msgs::action::PabxAnnouncement::Feedback>();
        feedback->call_status = call_status_;
        goal_handle->publish_feedback(feedback);
    }

}


void SIPCall::setGoalHandle(std::shared_ptr<GoalHandleAnnouncementAction> goal_handle)
{
    goal_handle_ = goal_handle;
}


void SIPCall::onCallTransferStatus(pj::OnCallTransferStatusParam &prm)
{

    RCLCPP_INFO(LOGGER, "*** Transfer status code: %d ***", prm.statusCode);

    if (prm.statusCode >= 300) {
        std::cout << "Transfer failed!" << std::endl;
        RCLCPP_ERROR(LOGGER, "*** Transfer failed ***");
        call_status_ = "TRANSFER_FAILED";
    } else if (prm.statusCode >= 200) {
        RCLCPP_INFO(LOGGER, "*** Transfer success ***");
        call_status_ = "TRANSFER_SUCCESS";
        transfer_success_ = true;
    } else {
        RCLCPP_INFO(LOGGER, "*** Transfer in progress ***");
        call_status_ = "TRANSFER_IN_PROGRESS";
    }

    // Publish feedback if goal_handle is valid
    auto goal_handle = goal_handle_.lock();
    if (goal_handle) {
        auto feedback = std::make_shared<kabam_msgs::action::PabxAnnouncement::Feedback>();
        feedback->call_status = call_status_;
        goal_handle->publish_feedback(feedback);
    }

}

std::string SIPCall::getStatus()
{
    return call_status_;
}   

bool SIPCall::transferSuccess() const
{
    return transfer_success_;
}