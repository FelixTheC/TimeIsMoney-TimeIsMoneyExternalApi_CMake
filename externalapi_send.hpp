//
// Created by felix on 29.10.23.
//

#ifndef TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_SEND_HPP
#define TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_SEND_HPP

#include <tuple>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QSqlDatabase>
#include <QSharedPointer>

#include "externalapi_db.hpp"

enum class SendTopic
{
    Start,
    Pause,
    Stop
};

enum class ContentType
{
    form_urlencoded,
    form_data,
    binary,
    raw_json,
    raw_text,
    raw_xml
};

enum class Auth
{
    None,
    Basic,
    Bearer
};

inline std::string auth_repr(Auth &auth)
{
    switch (auth)
    {
        case Auth::None:
            return "None";
        case Auth::Basic:
            return "Basic";
        case Auth::Bearer:
            return "Bearer";
    }
}

[[ nodiscard ]] QNetworkRequest get_network_request(QString &&url, const ContentType &type) noexcept;
[[ nodiscard ]] QByteArray get_request_data(QJsonValue &&json_val,
                                            QNetworkRequest &net_req,
                                            const ContentType &type,
                                            const std::tuple<const Auth, const std::string> &api_kind,
                                            const std::string &token);
[[ nodiscard ]] std::tuple<const Auth, const std::string> get_api_kind(const std::string &api_kind_str);

/// @brief Send a start ot stop cmd to the external API.
/// @param db pointer to a QSqlDatabase
/// @param net_manager the QNetworkAccessManager as parameter to be able to use signals.
/// @param send_topic send either `start`, `pause` or `stop` to external api
/// @param type the header type of the request
void send_cmd(QSharedPointer<QSqlDatabase> &db,
              QSharedPointer<QNetworkAccessManager> &net_manager,
              const SendTopic &send_topic);

#endif //TIMEISMONEYEXTERNALAPI_CMAKE_EXTERNALAPI_SEND_HPP
