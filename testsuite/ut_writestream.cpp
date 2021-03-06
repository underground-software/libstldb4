/******************************************************************************
*******************************************************************************
*******************************************************************************

    Copyright (C) 2003 Ben Martin

    This file is part of libstldb4.

    libstldb4 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libstldb4 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libstldb4.  If not, see <http://www.gnu.org/licenses/>.

    For more details see the COPYING file in the root directory of this
    distribution.

    $Id: ut_writestream.cpp,v 1.3 2010/09/24 05:15:39 ben Exp $

*******************************************************************************
*******************************************************************************
******************************************************************************/

/*
 * return 0 for success
 * return 1 for generic error
*/

#include <STLdb4/stldb4.hh>

#include <popt.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <iterator>
#include <iostream>

using namespace std;
using namespace STLdb4;
using namespace Ferris;

const string PROGRAM_NAME = "ut_writestream";

void usage(poptContext optCon, int exitcode, char *error, char *addl)
{
    poptPrintUsage(optCon, stderr, 0);
    if (error) fprintf(stderr, "%s: %s0", error, addl);
    exit(exitcode);
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

int errors = 0;

void runtest()
{
    fh_database db = new Database( DB_BTREE, "/tmp/play.db" );

    string data = "1234567890";
    db[ "fred" ] = data;
    cerr << "Initial value:" << db["fred"] << endl;
    
    {
        fh_iostream ss = db->getIOStream( "fred" );
        ss << "54321";
    }

    cerr << "Mid stange value:" << db["fred"] << endl;
    
    {
        fh_iostream ss = db->getIOStream( "fred" );
        ss.seekp( 3 );
        ss << "AAAA";
    }

    string wanted = "543AAAA890";
    string value  = db["fred"];
    cerr << "Final value:" << value << endl;
    if( value != wanted )
    {
        ++errors;
        cerr << "ERROR: didn't get the correct result:" << wanted << endl;
    }


    
    wanted = "sm";
    {
        fh_iostream oss = db->find( "fred" ).getIOStream( ios::trunc, 0 );
        oss << wanted;
    }
    value  = db["fred"];
    cerr << "Trunc and write final value:" << value << endl;
    if( value != wanted )
    {
        ++errors;
        cerr << "ERROR: didn't get the correct result after trunc+write:" << wanted << endl;
    }


    wanted = "smAndMore";
    {
        fh_iostream oss = db->find( "fred" ).getIOStream( ios::ate, 0 );
        oss << "AndMore";
    }
    value  = db["fred"];
    cerr << "Trunc and write final value:" << value << endl;
    if( value != wanted )
    {
        ++errors;
        cerr << "ERROR: didn't get the correct result after trunc+write:" << wanted << endl;
    }
    
    
}


int main( int argc, char** argv )
{
    int exit_status = 0;
    
    try
    {
        unsigned long Verbose              = 0;

        struct poptOption optionsTable[] =
            {
                { "verbose", 'v', POPT_ARG_NONE, &Verbose, 0,
                  "show what is happening", "" },

                POPT_AUTOHELP
                POPT_TABLEEND
            };
        poptContext optCon;

        optCon = poptGetContext(PROGRAM_NAME.c_str(), argc, (const char**)argv, optionsTable, 0);
        poptSetOtherOptionHelp(optCon, "[OPTIONS]*  ...");

        /* Now do options processing */
        char c=-1;
        while ((c = poptGetNextOpt(optCon)) >= 0)
        {}

        runtest();
    }
    catch( exception& e )
    {
        cerr << "cought error:" << e.what() << endl;
        exit(1);
    }
    if( !errors )
        cerr << "Success" << endl;
    return exit_status;
}
