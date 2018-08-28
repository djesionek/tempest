#ifndef DNSServer_h
#define DNSServer_h
#include <WiFiUdp.h>

#define DNS_QR_QUERY 0
#define DNS_QR_RESPONSE 1
#define DNS_OPCODE_QUERY 0

#define MAX_DNS_ENTRIES 20

enum class DNSReplyCode
{
  NoError = 0,
  FormError = 1,
  ServerFailure = 2,
  NonExistentDomain = 3,
  NotImplemented = 4,
  Refused = 5,
  YXDomain = 6,
  YXRRSet = 7,
  NXRRSet = 8
};

struct DNSHeader
{
  uint16_t ID;               // identification number
  unsigned char RD : 1;      // recursion desired
  unsigned char TC : 1;      // truncated message
  unsigned char AA : 1;      // authoritive answer
  unsigned char OPCode : 4;  // message_type
  unsigned char QR : 1;      // query/response flag
  unsigned char RCode : 4;   // response code
  unsigned char Z : 3;       // its z! reserved
  unsigned char RA : 1;      // recursion available
  uint16_t QDCount;          // number of question entries
  uint16_t ANCount;          // number of answer entries
  uint16_t NSCount;          // number of authority entries
  uint16_t ARCount;          // number of resource entries
};

class DNSEntry
{
public:
  DNSEntry(const String &domain, const IPAddress &ip){
    _domainName = domain;
    _resolveIP[0] = ip[0];
    _resolveIP[1] = ip[1];
    _resolveIP[2] = ip[2];
    _resolveIP[3] = ip[3];
  }

  String & getDomain(){
    return _domainName;
  }
  
  unsigned char* getIPAddress(){
    return _resolveIP;
  }

private:
  String _domainName;
  unsigned char _resolveIP[4];
};

class DNSServer
{
  public:
    DNSServer();
    void processNextRequest();
    void setErrorReplyCode(const DNSReplyCode &replyCode);
    void setTTL(const uint32_t &ttl);

    // Returns true if successful, false if there are no sockets available
    bool start(const uint16_t &port);
    bool addEntry(uint8 index, const String &domainName,
                     const IPAddress &resolvedIP);
    // stops the DNS server
    void stop();

  private:
    WiFiUDP _udp;
    uint16_t _port;
    //String _domainName;
    //unsigned char _resolvedIP[4];
    DNSEntry* _dnsEntries[MAX_DNS_ENTRIES];
    int _currentPacketSize;
    unsigned char* _buffer;
    DNSHeader* _dnsHeader;
    uint32_t _ttl;
    DNSReplyCode _errorReplyCode;

    void downcaseAndRemoveWwwPrefix(String &domainName);
    String getDomainNameWithoutWwwPrefix();
    bool requestIncludesOnlyOneQuestion();
    void replyWithIP(unsigned char*);
    void replyWithCustomCode();
};
#endif