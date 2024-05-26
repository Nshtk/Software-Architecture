#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <istream>
#include <ostream>

#include "Poco/Base64Decoder.h"
#include "Poco/JWT/Token.h"
#include "Poco/JWT/Signer.h"

bool getIdentity(const std::string identity, std::string &login, std::string &password);
std::string getTokenSecret();
std::string generateToken(long &id, std::string &login);
bool extractPayload(std::string &jwt_token, long &id, std::string &login);

#endif