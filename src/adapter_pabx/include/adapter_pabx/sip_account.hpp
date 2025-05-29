#ifndef ADAPTER_PABX_SIP_ACCOUNT_HPP
#define ADAPTER_PABX_SIP_ACCOUNT_HPP

/**
 * @file adapter_pabx/sip_account.hpp
 * @brief SIPAccount class that provides account management for PABX announcements.
 * @author V Mohammed Ibrahim (ibrahim.mohammed@kabam.ai)
 * @date 2025-05-26
 */

#include <pjsua2.hpp>


class SIPAccount : public pj::Account
{

private:

    bool accountRegistered_ = false; ///< Flag to indicate if the account is registered

    std::string username_;
    std::string password_;
    std::string domain_;
    pj::AccountConfig account_config_;
    pj::AuthCredInfo auth_cred_info_;

public:

    /**
     * @brief Registers a SIP account with the given credentials and domain.
     * @param username The SIP username (extension).
     * @param password The SIP password.
     * @param domain The SIP domain or server IP.
     * @return True if registration is successful, false otherwise.
     */
    bool registerAccount(std::string username, std::string password, std::string domain);

    /**
     * @brief Get the username associated with this SIP account.
     * @return The SIP username.
     */
    std::string getUsername();

    /**
     * @brief Get the domain associated with this SIP account.
     * @return The SIP domain.
     */
    std::string getDomain();

    /**
     * @brief Callback for registration state changes.
     * @param prm Registration state parameters (unused).
     */
    void onRegState(pj::OnRegStateParam &prm [[maybe_unused]]);

};

#endif // ADAPTER_PABX_SIP_ACCOUNT_HPP