#include "utility.h"

bool getIdentity(const std::string identity, std::string &login, std::string &password)
{
    std::istringstream istr(identity);
    std::ostringstream ostr;
    Poco::Base64Decoder b64in(istr);
    copy(std::istreambuf_iterator<char>(b64in), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(ostr));
    std::string decoded = ostr.str();

    size_t pos = decoded.find(':');
    login = decoded.substr(0, pos);
    password = decoded.substr(pos + 1);
	
    return true;
}
std::string getTokenSecret() 
{
    return "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE3MTI2OTEyMDguNDM4MjI5LCJpZCI6MSwibG9naW4iOiJzYW1wbGVfbG9naW5fMSIsInN1YiI6ImxvZ2luIn0";
}
std::string generateToken(long &id, std::string &login) 
{
    Poco::JWT::Token token;
    token.setType("JWT");
    token.setSubject("login");
    token.payload().set("login", login);
    token.payload().set("id", id);
    token.setIssuedAt(Poco::Timestamp());

    Poco::JWT::Signer signer(getTokenSecret());
    return signer.sign(token, Poco::JWT::Signer::ALGO_HS256);
}
bool extractPayload(std::string &jwt_token, long &id, std::string &login) 
{
    if (jwt_token.length() == 0) 
        return false;

    Poco::JWT::Signer signer(getTokenSecret());
    try 
	{
        Poco::JWT::Token token = signer.verify(jwt_token);
        if (token.payload().has("login") && token.payload().has("id")) 
		{
            login = token.payload().getValue<std::string>("login");
            id = token.payload().getValue<long>("id");	
            return true;		//TODO verify via db
        }
        std::cout << "Not enough fields in token" << std::endl;
    } 
	catch (...) 
	{
        std::cout << "Token verification failed" << std::endl;
    }
    return false;
}