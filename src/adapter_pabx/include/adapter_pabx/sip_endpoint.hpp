#ifndef ADAPTER_PABX_SIP_ENDPOINT_HPP
#define ADAPTER_PABX_SIP_ENDPOINT_HPP

#include <pjsua2.hpp>

class SIPEndpoint : public pj::Endpoint
{

public:

    /**
     * @brief Constructor for SIPEndpoint.
     */
    SIPEndpoint();

    /**
     * @brief Destructor for SIPEndpoint.
     */
    ~SIPEndpoint();


    /**
     * @brief Initializes the SIP endpoint with the specified console log level.
     *
     * This method configures the endpoint's logging and media settings, and prepares it for operation.
     * @param console_level The log level for console output (default: 0).
     */
    void init(int console_level = 0);

    /**
     * @brief Starts the SIP endpoint and sets up the transport.
     *
     * This method starts the endpoint and configures the audio device to null to avoid playback.
     */
    void start();

    /**
     * @brief Shuts down the SIP endpoint and releases resources.
     */
    void stop();

private:

    pj::EpConfig endpoint_config_;
    pj::TransportConfig transport_config_;

};


#endif