// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry               
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2020.
// 
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution 
//    may be used to endorse or promote products derived from this software 
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS. 
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING 
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// --------------------------------------------------------------------------
// $Maintainer: Chris Bielow $
// $Authors: Chris Bielow $
// --------------------------------------------------------------------------

#include <OpenMS/CONCEPT/ClassTest.h>
#include <OpenMS/test_config.h>

///////////////////////////
#include <OpenMS/FORMAT/OSWFile.h>
///////////////////////////

using namespace OpenMS;
using namespace std;

START_TEST(OSWFile, "$Id$")

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/*
Creating an OWS test database from pyProphet osw-db files (which provided the SCORE_MS2 table, which is missing in OpenSwathWorkflow outputs)

Using DBBrowser for SQLite:
1) open a full blown OSW file
2) export the DB schema to a new test DB file (without the data)
3) To fill the empty test DB, we start at the highest (=Protein) level and pick all dependent rows from downstream tables using the SQL-commands below.
The resulting data can be copied "as SQL-commands" and inserted into the test DB. This has to be done table by table.
Sometimes, only the first columns must be selected using the mouse, because the query contains more columns from joins with other tables.
Note: not all tables are populated. Only the ones we need at the moment..

SELECT * FROM PROTEIN WHERE ID IN (2,3866)
--> insert data into PROTEIN table

select * from PEPTIDE
INNER JOIN (SELECT PEPTIDE_ID as PID, PROTEIN_ID as POD FROM PEPTIDE_PROTEIN_MAPPING) AS PEP ON PEP.PID=PEPTIDE.ID WHERE PEP.POD IN (2,3866)
--> insert data into PEPTIDE table

SELECT * FROM PEPTIDE_PROTEIN_MAPPING WHERE PROTEIN_ID IN (2,3866)
--> insert data into PEPTIDE_PROTEIN_MAPPING table


select PEPTIDE_ID, PRECURSOR_ID FROM PRECURSOR_PEPTIDE_MAPPING
INNER JOIN (SELECT PEPTIDE_ID as PID, PROTEIN_ID as POD FROM PEPTIDE_PROTEIN_MAPPING) AS PEP ON PEP.PID=PEPTIDE_ID WHERE PEP.POD IN (2,3866) AND PRECURSOR_PEPTIDE_MAPPING.PEPTIDE_ID=PEP.PID
--> insert data into PRECURSOR_PEPTIDE_MAPPING table

SELECT * FROM PRECURSOR
INNER JOIN (select PEPTIDE_ID, PRECURSOR_ID FROM PRECURSOR_PEPTIDE_MAPPING
INNER JOIN (SELECT PEPTIDE_ID as PID, PROTEIN_ID as POD FROM PEPTIDE_PROTEIN_MAPPING) AS PEP ON PEP.PID=PEPTIDE_ID WHERE PEP.POD IN (2,3866) AND PRECURSOR_PEPTIDE_MAPPING.PEPTIDE_ID=PEP.PID) as PC ON PC.PRECURSOR_ID=PRECURSOR.ID
--> insert data into PRECURSOR table

SELECT * from FEATURE
INNER JOIN (SELECT * FROM PRECURSOR
INNER JOIN (select PEPTIDE_ID, PRECURSOR_ID FROM PRECURSOR_PEPTIDE_MAPPING
INNER JOIN (SELECT PEPTIDE_ID as PID, PROTEIN_ID as POD FROM PEPTIDE_PROTEIN_MAPPING) AS PEP ON PEP.PID=PEPTIDE_ID WHERE PEP.POD IN (2,3866) AND PRECURSOR_PEPTIDE_MAPPING.PEPTIDE_ID=PEP.PID) as PC ON PC.PRECURSOR_ID=PRECURSOR.ID) AS PREC ON PREC.ID=FEATURE.PRECURSOR_ID
--> insert data into FEATURE table

SELECT * from FEATURE_TRANSITION
INNER JOIN (SELECT * from FEATURE
INNER JOIN (SELECT * FROM PRECURSOR
INNER JOIN (select PEPTIDE_ID, PRECURSOR_ID FROM PRECURSOR_PEPTIDE_MAPPING
INNER JOIN (SELECT PEPTIDE_ID as PID, PROTEIN_ID as POD FROM PEPTIDE_PROTEIN_MAPPING) AS PEP ON PEP.PID=PEPTIDE_ID WHERE PEP.POD IN (2,3866) AND PRECURSOR_PEPTIDE_MAPPING.PEPTIDE_ID=PEP.PID) as PC ON PC.PRECURSOR_ID=PRECURSOR.ID) AS PREC ON PREC.ID=FEATURE.PRECURSOR_ID) AS FEAT ON FEAT.ID=FEATURE_TRANSITION.FEATURE_ID
--> insert data into FEATURE_TRANSITION table

SELECT *  from TRANSITION
INNER JOIN (SELECT DISTINCT TRANSITION_ID from FEATURE_TRANSITION
INNER JOIN (SELECT * from FEATURE
INNER JOIN (SELECT * FROM PRECURSOR
INNER JOIN (select PEPTIDE_ID, PRECURSOR_ID FROM PRECURSOR_PEPTIDE_MAPPING
INNER JOIN (SELECT PEPTIDE_ID as PID, PROTEIN_ID as POD FROM PEPTIDE_PROTEIN_MAPPING) AS PEP ON PEP.PID=PEPTIDE_ID WHERE PEP.POD IN (2,3866) AND PRECURSOR_PEPTIDE_MAPPING.PEPTIDE_ID=PEP.PID) as PC ON PC.PRECURSOR_ID=PRECURSOR.ID) AS PREC ON PREC.ID=FEATURE.PRECURSOR_ID) AS FEAT ON FEAT.ID=FEATURE_TRANSITION.FEATURE_ID) AS FEATTR ON FEATTR.TRANSITION_ID=TRANSITION.ID
--> insert data into TRANSITION table

SELECT * from SCORE_MS2
INNER JOIN (SELECT * from FEATURE
INNER JOIN (SELECT * FROM PRECURSOR
INNER JOIN (select PEPTIDE_ID, PRECURSOR_ID FROM PRECURSOR_PEPTIDE_MAPPING
INNER JOIN (SELECT PEPTIDE_ID as PID, PROTEIN_ID as POD FROM PEPTIDE_PROTEIN_MAPPING) AS PEP ON PEP.PID=PEPTIDE_ID WHERE PEP.POD IN (2,3866) AND PRECURSOR_PEPTIDE_MAPPING.PEPTIDE_ID=PEP.PID) as PC ON PC.PRECURSOR_ID=PRECURSOR.ID) AS PREC ON PREC.ID=FEATURE.PRECURSOR_ID)
		AS FEAT ON FEAT.ID=SCORE_MS2.FEATURE_ID
--> insert data into SCORE_MS2 table  (only available after pyProphet ran!)


*/

START_SECTION(static void read(const std::string& filename, OSWData& swath_result))
	
	OSWData res;
	OSWFile::read(OPENMS_GET_TEST_DATA_PATH("OSWFile_test.osw"), res);

	TEST_EQUAL(res.protCount(), 2);
	TEST_EQUAL(res.transitionCount(), 700);

	OSWProtein prot = *res.protBegin();
	TEST_EQUAL(prot.accession_, "1/P00167ups|CYB5_HUMAN_UPS");
	OSWPeptidePrecursor prec = *prot.peptides_.begin();
	TEST_EQUAL(prec.charge_, 2);
	TEST_EQUAL(prec.decoy_, false);
	TEST_REAL_SIMILAR(prec.precursor_mz_, 1103.4676);
	TEST_EQUAL(prec.seq_, "EQAGGDATENFEDVGHSTDAR");


			
END_SECTION			

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
END_TEST



