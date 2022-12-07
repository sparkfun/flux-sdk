
/*
 * Header file with AWS parameters for my devices
 */

#pragma once
#include <pgmspace.h>
// Items needed to communicate with a defined "Thing" on AWS IoT

// TODO  - FILL in with info for target AWS IoT Thing

// The "name" of the Thing
const  char kAWSMyThingName[] = "";

// The AWS mqtt endpoint to connect to
const char kAWSIOTEndpoint[] = 	"";

// Certificates for our AWS endpont - these are provisioned with the thing 

// Amazon Root CA 1
static const char kAWSCertCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char kAWSCertCRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char kAWSCertPrivate[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";