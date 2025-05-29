#ifndef ADAPTER_PABX_SIP_CALL_HPP
#define ADAPTER_PABX_SIP_CALL_HPP


#include <pjsua2.hpp>
#include <kabam_msgs/action/pabx_announcement.hpp>
#include <rclcpp_action/rclcpp_action.hpp>

class SIPCall : public pj::Call
{

public:

    using GoalHandleAnnouncementAction = rclcpp_action::ServerGoalHandle<kabam_msgs::action::PabxAnnouncement>;

    /**
     * @brief Constructor for the SIPCall class.
     * @param account The SIP account associated with this call.
     * @param call_id The unique identifier for the call.
     */
    SIPCall(pj::Account &account, int call_id=PJSUA_INVALID_ID);

    /**
     * @brief Destructor for the SIPCall class.
     */
    ~SIPCall();

    /**
     * @brief Initiates a call to the specified destination URI and sets up transfer.
     * @param dest_uri The destination SIP URI to call.
     * @param transfer_uri The SIP URI to transfer the call to after connection.
     */
    void makeCall(const std::string &dest_uri, std::string transfer_uri);

    /**
     * @brief Callback for call state changes (e.g., ringing, connected, disconnected).
     * @param prm Call state parameters (unused).
     */
    void onCallState(pj::OnCallStateParam &prm [[maybe_unused]]);

    /**
     * @brief Set the goal handle for action feedback during the call.
     * @param goal_handle Shared pointer to the action goal handle.
     */
    void setGoalHandle(std::shared_ptr<GoalHandleAnnouncementAction> goal_handle);

    /**
     * @brief Callback for call transfer status updates.
     * @param prm Call transfer status parameters.
     */
    void onCallTransferStatus(pj::OnCallTransferStatusParam &prm);

    /**
     * @brief Get the current status of the call as a string.
     * @return The call status (e.g., RINGING, CONNECTED, DISCONNECTED).
     */
    std::string getStatus();

    /**
     * @brief Check if the call transfer was successful.
     * @return True if transfer succeeded, false otherwise.
     */
    bool transferSuccess() const;

private:

    std::string transfer_uri_ = "";

    bool transfer_success_ = false; ///< Flag to indicate if transfer has been attempted

    std::string call_status_ = "UNKNOWN"; ///< Current call status
    pjsip_inv_state call_state_ = pjsip_inv_state::PJSIP_INV_STATE_NULL; ///< Current state of the call

    pj::CallOpParam call_param_; ///< Call operation parameters for making calls
    std::weak_ptr<GoalHandleAnnouncementAction> goal_handle_;
};


#endif // ADAPTER_PABX_SIP_CALL_HPP