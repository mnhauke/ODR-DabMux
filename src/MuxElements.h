/*
   Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,
   2011, 2012 Her Majesty the Queen in Right of Canada (Communications
   Research Center Canada)

   Copyright (C) 2014
   Matthias P. Braendli, matthias.braendli@mpb.li

   This file defines all data structures used in DabMux to represent
   and save ensemble data.
   */
/*
   This file is part of ODR-DabMux.

   ODR-DabMux is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   ODR-DabMux is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ODR-DabMux.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _MUX_ELEMENTS
#define _MUX_ELEMENTS

#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <stdint.h>
#include "dabOutput/dabOutput.h"
#include "dabInput.h"
#include "RemoteControl.h"
#include "Eti.h"

struct dabOutput {
    dabOutput(const char* proto, const char* name) :
        outputProto(proto), outputName(name), output(NULL) { }

    // outputs are specified with outputProto://outputName
    // during config parsing
    std::string outputProto;
    std::string outputName;

    // later, the corresponding output is then created
    DabOutput* output;
};


class DabLabel
{
    public:
        /* Set a new label and short label.
         * returns:  0 on success
         *          -1 if the short_label is not a representable
         *          -2 if the short_label is too long
         *          -3 if the text is too long
         */
        int setLabel(const std::string& text, const std::string& short_label);

        /* Same as above, but sets the flag to 0xff00, truncating at 8
         * characters.
         *
         * returns:  0 on success
         *          -3 if the text is too long
         */
        int setLabel(const std::string& text);

        const char* text() const { return m_text; }
        uint16_t flag() const { return m_flag; }
        const std::string short_label() const;

    private:
        // In the DAB standard, the label is 16 chars.
        // We keep it here zero-terminated
        char m_text[17];
        uint16_t m_flag;
        int setShortLabel(const std::string& slabel);
};


class DabService;
class DabComponent;

struct dabSubchannel;
class dabEnsemble : public RemoteControllable {
    public:
        dabEnsemble()
            : RemoteControllable("ensemble")
        {
            RC_ADD_PARAMETER(localtimeoffset,
                    "local time offset, 'auto' or -24 to +24 [half-hours]");
        }

        /* Remote control */
        virtual void set_parameter(const std::string& parameter,
               const std::string& value);

        /* Getting a parameter always returns a string. */
        virtual const std::string get_parameter(const std::string& parameter) const;

        /* all fields are public, since this was a struct before */
        uint16_t id;
        uint8_t ecc;
        DabLabel label;
        uint8_t mode;

        /* Use the local time to calculate the lto */
        bool lto_auto;

        int lto; // local time offset in half-hours
        // range: -24 to +24

        int international_table;

        std::vector<DabService*> services;
        std::vector<DabComponent*> components;
        std::vector<dabSubchannel*> subchannels;
};


struct dabProtectionUEP {
    unsigned char tableIndex;
};

enum dab_protection_eep_profile {
    EEP_A,
    EEP_B
};

struct dabProtectionEEP {
    dab_protection_eep_profile profile;

    // option is a 3-bit field where 000 and 001 are used to
    // select EEP profile A and B.
    // Other values are for future use, see
    // EN 300 401 Clause 6.2.1 "Basic sub-channel organisation"
    uint8_t GetOption(void) const {
        return (this->profile == EEP_A) ? 0 : 1;
    }
};

enum dab_protection_form_t {
    UEP, // implies FIG0/1 Short form
    EEP  //                Long form
};

struct dabProtection {
    unsigned char level;
    dab_protection_form_t form;
    union {
        dabProtectionUEP uep;
        dabProtectionEEP eep;
    };
};

enum dab_subchannel_type_t {
    Audio = 0,
    DataDmb = 1,
    Fidc = 2,
    Packet = 3
};

struct dabSubchannel {
    std::string inputUri;
    DabInputBase* input;
    unsigned char id;
    dab_subchannel_type_t type;
    uint16_t startAddress;
    uint16_t bitrate;
    dabProtection protection;
};


class SubchannelId : public std::binary_function <dabSubchannel*, int, bool> {
public:
    bool operator()(const dabSubchannel* subchannel, const int id) const {
        return subchannel->id == id;
    }
};




struct dabAudioComponent {
    dabAudioComponent() :
        uaType(0xFFFF) {}

    uint16_t uaType; // User Application Type
};


struct dabDataComponent {
};


struct dabFidcComponent {
};


struct dabPacketComponent {
    dabPacketComponent() :
        id(0),
        address(0),
        appType(0xFFFF),
        datagroup(false) { }

    uint16_t id;
    uint16_t address;
    uint16_t appType;
    bool datagroup;
};


class DabComponent : public RemoteControllable
{
    public:
        DabComponent(std::string uid) :
            RemoteControllable(uid)
        {
            RC_ADD_PARAMETER(label, "Label and shortlabel [label,short]");
        }

        DabLabel label;
        uint32_t serviceId;
        uint8_t subchId;
        uint8_t type;
        uint8_t SCIdS;

        dabAudioComponent audio;
        dabDataComponent data;
        dabFidcComponent fidc;
        dabPacketComponent packet;

        bool isPacketComponent(std::vector<dabSubchannel*>& subchannels);

        /* Remote control */
        virtual void set_parameter(const std::string& parameter,
               const std::string& value);

        /* Getting a parameter always returns a string. */
        virtual const std::string get_parameter(const std::string& parameter) const;

        virtual ~DabComponent() {}

    private:
        const DabComponent& operator=(const DabComponent& other);
        DabComponent(const DabComponent& other);
};



class DabService : public RemoteControllable
{
    public:
        DabService(std::string uid) :
            RemoteControllable(uid)
        {
            RC_ADD_PARAMETER(label, "Label and shortlabel [label,short]");
        }

        uint32_t id;
        unsigned char pty;
        unsigned char language;
        bool program;

        unsigned char getType(dabEnsemble* ensemble);
        unsigned char nbComponent(std::vector<DabComponent*>& components);

        DabLabel label;

        /* Remote control */
        virtual void set_parameter(const std::string& parameter,
               const std::string& value);

        /* Getting a parameter always returns a string. */
        virtual const std::string get_parameter(const std::string& parameter) const;

        virtual ~DabService() {}

    private:
        const DabService& operator=(const DabService& other);
        DabService(const DabService& other);
};

std::vector<dabSubchannel*>::iterator getSubchannel(
        std::vector<dabSubchannel*>& subchannels, int id);

std::vector<DabComponent*>::iterator getComponent(
        std::vector<DabComponent*>& components,
        uint32_t serviceId,
        std::vector<DabComponent*>::iterator current);

std::vector<DabComponent*>::iterator getComponent(
        std::vector<DabComponent*>& components,
        uint32_t serviceId);

std::vector<DabService*>::iterator getService(
        DabComponent* component,
        std::vector<DabService*>& services);

unsigned short getSizeCu(dabSubchannel* subchannel);

unsigned short getSizeDWord(dabSubchannel* subchannel);

unsigned short getSizeByte(dabSubchannel* subchannel);

unsigned short getSizeWord(dabSubchannel* subchannel);


#endif
