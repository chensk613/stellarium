/*
 * Stellarium
 * Copyright (C) 2019 Alexander Wolf
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#include "tests/testComputations.hpp"

#include <QString>
#include <QDebug>
#include <QtGlobal>
#include <QVariantList>
#include <QMap>

#include "StelUtils.hpp"
#include "VecMath.hpp"

#define ERROR_LOW_LIMIT 1e-3
#define ERROR_MID_LIMIT 1e-4
#define ERROR_HIGH_LIMIT 1e-5

QTEST_GUILESS_MAIN(TestComputations)

void TestComputations::testSiderealPeriodComputations()
{
	QVariantList data;

	// According to WolframAlpha
	data << 1.00000011	<< 365.25636;	// Earth
	data << 0.38709893	<< 87.96926;	// Mercury
	data << 0.72333199	<< 224.7008;	// Venus

	while (data.count() >= 2)
	{
		double distance = data.takeFirst().toDouble();
		double exPeriod = data.takeFirst().toDouble();
		double period = StelUtils::calculateSiderealPeriod(distance);

		QVERIFY2(qAbs(period-exPeriod)<=ERROR_LOW_LIMIT, qPrintable(QString("Sidereal period is %1 days for %2 AU (expected %3 days)")
									.arg(QString::number(period, 'f', 6))
									.arg(QString::number(distance, 'f', 5))
									.arg(QString::number(exPeriod, 'f', 6))));
	}
}


void TestComputations::testJDFormBesselianEpoch()
{
	QVariantList data;

	// According to Observational Astrophysics by Pierre Lena, Francois Lebrun, Francois Mignard (ISBN 3662036851, 9783662036853)
	data << 1900.0		<< 2415020.3135;
	data << 1950.0		<< 2433282.4235;
	data << 1995.00048	<< 2449718.5;
	// FIXME: WTF???
	//data << 2000.0		<< 2451544.4334;
	//data << 1950.000210	<< 2433282.5;
	//data << 2000.001278	<< 2451545.0;

	while (data.count() >= 2)
	{
		double besselianEpoch	= data.takeFirst().toDouble();
		double expectedJD	= data.takeFirst().toDouble();
		double JD = StelUtils::getJDFromBesselianEpoch(besselianEpoch);

		QVERIFY2(qAbs(JD-expectedJD)<=ERROR_LOW_LIMIT, qPrintable(QString("Julian date for Besselian epoch %1 is %2 (expected %3)")
									.arg(QString::number(besselianEpoch, 'f', 6))
									.arg(QString::number(JD, 'f', 4))
									.arg(QString::number(expectedJD, 'f', 4))));
	}
}

void TestComputations::testIsPowerOfTwo()
{
	int n;
	bool f, r;
	QVariantList data;
	data	<< 0	<< true;
	data	<< 1	<< true;
	data	<< 2	<< true;
	data	<< 3	<< false;
	data	<< 4	<< true;
	data	<< 5	<< false;
	data	<< 6	<< false;
	data	<< 7	<< false;
	data	<< 8	<< true;
	data	<< 9	<< false;

	while (data.count() >= 2)
	{
		n	= data.takeFirst().toInt();
		f	= data.takeFirst().toBool();

		r = StelUtils::isPowerOfTwo(n);

		QVERIFY2(r==f, qPrintable(QString("%1 %2 power of two (expected: %3)")
					   .arg(n)
					   .arg(r ? "is":"is not")
					   .arg(f)));
	}
}

void TestComputations::testDayInYear()
{
	int year, month, day, en, n;

	QVariantList data;
	data	<< 2000	<< 1	<< 1	<< 1;
	data	<< 2019	<< 1	<< 1	<< 1;
	data	<< 2019	<< 1	<< 31	<< 31;
	data	<< 2019	<< 2	<< 28	<< 59;
	data	<< 2019	<< 3	<< 10	<< 69;
	data	<< 2020	<< 3	<< 10	<< 70;

	while (data.count() >= 4)
	{
		year	= data.takeFirst().toInt();
		month	= data.takeFirst().toInt();
		day	= data.takeFirst().toInt();
		en	= data.takeFirst().toInt();

		n = StelUtils::dayInYear(year, month, day);

		QVERIFY2(n==en, qPrintable(QString("%1-%2-%3 is %4 day (expected: %5)")
					   .arg(year)
					   .arg(month)
					   .arg(day)
					   .arg(n)
					   .arg(en)));
	}
}

void TestComputations::testYearFraction()
{
	int year, month, day;
	double fraction, efrac;

	QVariantList data;
	data	<< 2000	<< 1	<< 1	<< 2000.00274;
	data	<< 2019	<< 1	<< 1	<< 2019.00274;
	data	<< 2019	<< 1	<< 31	<< 2019.08493;
	data	<< 2019	<< 2	<< 28	<< 2019.16164;
	data	<< 2019	<< 3	<< 10	<< 2019.18904;
	data	<< 2020	<< 3	<< 10	<< 2020.19126;

	while (data.count() >= 4)
	{
		year	= data.takeFirst().toInt();
		month	= data.takeFirst().toInt();
		day	= data.takeFirst().toInt();
		efrac	= data.takeFirst().toDouble();

		fraction = StelUtils::yearFraction(year, month, day);

		QVERIFY2(qAbs(fraction-efrac)<=ERROR_HIGH_LIMIT, qPrintable(QString("%1-%2-%3 is %4 (expected: %5)")
					   .arg(year)
					   .arg(month)
					   .arg(day)
					   .arg(QString::number(fraction, 'f', 5))
					   .arg(QString::number(efrac, 'f', 5))));
	}
}

void TestComputations::testEquToEqlTransformations()
{
	double eps0 = 23.4392911*M_PI/180.;
	double ra, dec, lambdaE, lambda, betaE, beta;

	QVariantList data;
	//                  RA                   DE                     Lambda           Beta
	data <<     0.		<<    0.		<<     0.		<<    0.;
	data <<   12.		<<  45.		<<   31.03451	<<  36.17676;
	data <<     2.5		<<  23.5		<<   12.03425	<<  20.48324;
	data <<   25.1225	<< -46.0025	<<    -1.41879	<< -51.01554;
	data <<  -60.		<<  60.		<<  -11.92479	<<  75.19595;

	while (data.count() >= 4)
	{
		ra	= data.takeFirst().toDouble();
		dec	= data.takeFirst().toDouble();
		lambdaE	= data.takeFirst().toDouble();
		betaE	= data.takeFirst().toDouble();

		StelUtils::equToEcl(ra*M_PI/180., dec*M_PI/180., eps0, &lambda, &beta);

		lambda *= 180/M_PI;
		beta *= 180/M_PI;

		QVERIFY2(qAbs(lambda-lambdaE)<=ERROR_LOW_LIMIT && qAbs(lambda-lambdaE)<=ERROR_LOW_LIMIT,
				qPrintable(QString("RA/Dec: %1/%2 Lam/Bet: %3/%4 (expected Lam/Bet: %5/%6)")
					   .arg(QString::number(ra, 'f', 5))
					   .arg(QString::number(dec, 'f', 5))
					   .arg(QString::number(lambda, 'f', 5))
					   .arg(QString::number(beta, 'f', 5))
					   .arg(QString::number(lambdaE, 'f', 5))
					   .arg(QString::number(betaE, 'f', 5))));
	}
}

void TestComputations::testEclToEquTransformations()
{
	double eps0 = 23.4392911*M_PI/180.;
	double ra, dec, raE, decE, lambda, beta;

	QVariantList data;
	//                  RA                   DE                     Lambda           Beta
	data <<     0.		<<    0.		<<     0.		<<    0.;
	data <<   12.		<<  45.		<<   31.03451	<<  36.17676;
	data <<     2.5		<<  23.5		<<   12.03425	<<  20.48324;
	data <<   25.1225	<< -46.0025	<<    -1.41879	<< -51.01554;
	data <<  -60.		<<  60.		<<  -11.92479	<<  75.19595;

	while (data.count() >= 4)
	{
		raE	= data.takeFirst().toDouble();
		decE	= data.takeFirst().toDouble();
		lambda	= data.takeFirst().toDouble();
		beta	= data.takeFirst().toDouble();

		StelUtils::eclToEqu(lambda*M_PI/180., beta*M_PI/180., eps0, &ra, &dec);

		ra *= 180/M_PI;
		dec *= 180/M_PI;

		QVERIFY2(qAbs(ra-raE)<=ERROR_LOW_LIMIT && qAbs(dec-decE)<=ERROR_LOW_LIMIT,
				qPrintable(QString("Lam/Bet: %1/%2 RA/Dec: %3/%4 (expected RA/Dec: %5/%6)")
					   .arg(QString::number(lambda, 'f', 5))
					   .arg(QString::number(beta, 'f', 5))
					   .arg(QString::number(ra, 'f', 5))
					   .arg(QString::number(dec, 'f', 5))
					   .arg(QString::number(raE, 'f', 5))
					   .arg(QString::number(decE, 'f', 5))));
	}
}

void TestComputations::testSpheToRectTransformations()
{
	float longitude, latitude;
	Vec3f eVec3f, rVec3f;
	Vec3d eVec3d, rVec3d;

	QVariantList data;
	//     Longitude          Latitude         Expected
	data <<     0.		<<   0.		<< "1,0,0";
	data <<    90.		<<   0.		<< "0,1,0";
	data <<     0.		<<  90.		<< "0,0,1";
	data <<   -90.		<<  90.		<< "0,0,1";
	data <<   180.		<< -90.		<< "0,0,-1";
	data <<   270.		<< -90.		<< "0,0,-1";
	data <<   -90.		<<  45.		<< "0,-0.7071,0.7071";

	while (data.count() >= 3)
	{
		longitude	= data.takeFirst().toFloat();
		latitude	= data.takeFirst().toFloat();
		eVec3f	= StelUtils::strToVec3f(data.takeFirst().toString());
		eVec3d	= eVec3f.toVec3d();

		StelUtils::spheToRect(longitude*M_PI/180.f, latitude*M_PI/180.f, rVec3f);
		StelUtils::spheToRect(longitude*M_PI/180., latitude*M_PI/180., rVec3d);

		QVERIFY2(qAbs(rVec3f[0]-eVec3f[0])<=ERROR_HIGH_LIMIT && qAbs(rVec3f[1]-eVec3f[1])<=ERROR_HIGH_LIMIT && qAbs(rVec3f[2]-eVec3f[2])<=ERROR_HIGH_LIMIT,
				qPrintable(QString("Long/Lat: %1/%2 = %3 (expected %4)")
					   .arg(QString::number(longitude, 'f', 4))
					   .arg(QString::number(latitude, 'f', 4))
					   .arg(rVec3f.toString())
					   .arg(eVec3f.toString())));

		QVERIFY2(qAbs(rVec3d[0]-eVec3d[0])<=ERROR_HIGH_LIMIT && qAbs(rVec3d[1]-eVec3d[1])<=ERROR_HIGH_LIMIT && qAbs(rVec3d[2]-eVec3d[2])<=ERROR_HIGH_LIMIT,
				qPrintable(QString("Long/Lat: %1/%2 = %3 (expected %4)")
					   .arg(QString::number(longitude, 'f', 4))
					   .arg(QString::number(latitude, 'f', 4))
					   .arg(rVec3d.toString())
					   .arg(eVec3d.toString())));
	}
}

void TestComputations::testRectToSpheTransformations()
{
	float longitude, latitude, longitudef, latitudef, longitudeE, latitudeE;
	double longituded, latituded, longitudes, latitudes;
	Vec3f rVec3f;
	Vec3d rVec3d;

	QVariantList data;
	//     Longitude          Latitude         Source
	data <<     0.		<<   0.		<< "1,0,0";
	data <<    90.		<<   0.		<< "0,1,0";
	data <<     0.		<<  90.		<< "0,0,1";
	data <<     0.		<< -90.		<< "0,0,-1";
	data <<   -90.		<<  45.		<< "0,-0.7071,0.7071";

	while (data.count() >= 3)
	{
		longitudeE	= data.takeFirst().toFloat();
		latitudeE		= data.takeFirst().toFloat();
		rVec3f		= StelUtils::strToVec3f(data.takeFirst().toString());
		rVec3d		= rVec3f.toVec3d();

		StelUtils::rectToSphe(&longitude, &latitude, rVec3f);
		StelUtils::rectToSphe(&longitudef, &latitudef, rVec3d);
		StelUtils::rectToSphe(&longituded, &latituded, rVec3d);
		StelUtils::rectToSphe(&longitudes, &latitudes, rVec3f);

		longitude		*= 180.f/M_PI;
		longitudef		*= 180.f/M_PI;
		longituded	*= 180./M_PI;
		longitudes	*= 180./M_PI;
		latitude		*= 180.f/M_PI;
		latitudef		*= 180.f/M_PI;
		latituded		*= 180./M_PI;
		latitudes		*= 180./M_PI;

		QVERIFY2(qAbs(longitude-longitudeE)<=ERROR_HIGH_LIMIT && qAbs(latitude-latitudeE)<=ERROR_HIGH_LIMIT,
				qPrintable(QString("Vec3 %1 = Long/Lat: %2/%3 (expected %4/%5)")
					   .arg(rVec3f.toString())
					   .arg(QString::number(longitude, 'f', 4))
					   .arg(QString::number(latitude, 'f', 4))
					   .arg(QString::number(longitudeE, 'f', 4))
					   .arg(QString::number(latitudeE, 'f', 4))));

		QVERIFY2(qAbs(longitudef-longitudeE)<=ERROR_HIGH_LIMIT && qAbs(latitudef-latitudeE)<=ERROR_HIGH_LIMIT,
				qPrintable(QString("Vec3 %1 = Long/Lat: %2/%3 (expected %4/%5)")
					   .arg(rVec3d.toString())
					   .arg(QString::number(longitudef, 'f', 4))
					   .arg(QString::number(latitudef, 'f', 4))
					   .arg(QString::number(longitudeE, 'f', 4))
					   .arg(QString::number(latitudeE, 'f', 4))));

		QVERIFY2(qAbs(longituded-(double)longitudeE)<=ERROR_HIGH_LIMIT && qAbs(latituded-(double)latitudeE)<=ERROR_HIGH_LIMIT,
				qPrintable(QString("Vec3 %1 = Long/Lat: %2/%3 (expected %4/%5)")
					   .arg(rVec3d.toString())
					   .arg(QString::number(longituded, 'f', 4))
					   .arg(QString::number(latituded, 'f', 4))
					   .arg(QString::number(longitudeE, 'f', 4))
					   .arg(QString::number(latitudeE, 'f', 4))));

		QVERIFY2(qAbs(longitudes-(double)longitudeE)<=ERROR_HIGH_LIMIT && qAbs(latitudes-(double)latitudeE)<=ERROR_HIGH_LIMIT,
				qPrintable(QString("Vec3 %1 = Long/Lat: %2/%3 (expected %4/%5)")
					   .arg(rVec3f.toString())
					   .arg(QString::number(longitudes, 'f', 4))
					   .arg(QString::number(latitudes, 'f', 4))
					   .arg(QString::number(longitudeE, 'f', 4))
					   .arg(QString::number(latitudeE, 'f', 4))));
	}
}

void TestComputations::testVector2Operators()
{
	Vec2f firstF, secondF, vecF, sumF, diffF, mulF, smF, cwmF;
	Vec2d firstD, secondD, vecD, sumD, diffD, mulD, smD, cwmD;
	Vec2i firstI, secondI, vecI, sumI, diffI, mulI, smI, cwmI;
	bool expected, result;
	QVariantList data;
	//           Vec(1)       Vec(2)      Compare   Sum         Diff                   Mul                  Scal. M.           C.W.M.
	data << "0,0"	<< "0,0"	<< true	<<  "0,0"	<<   "0,0"	<<   "0,0"	<<  "0,0"		<< "0,0";
	data << "0,0"	<< "1,0"	<< false	<<  "1,0"	<<  "-1,0"	<<   "0,0"	<<  "0,0"		<< "0,0";
	data << "0,0"	<< "1,1"	<< false	<<  "1,1"	<< "-1,-1"	<<   "0,0"	<<  "0,0"		<< "0,0";
	data << "1,1"	<< "0,0"	<< false	<<  "1,1"	<<   "1,1"	<<   "0,0"	<<  "2,2"		<< "1,1";
	data << "1,1"	<< "1,1"	<< true	<<  "2,2"	<<   "0,0"	<<   "1,1"	<<  "2,2"		<< "1,1";
	data << "1,-1"	<< "1,-1"	<< true	<< "2,-2"	<<   "0,0"	<<   "1,1"	<< "2,-2"		<< "1,1";
	data << "1,-1"	<< "-1,1"	<< false	<<  "0,0"	<<   "2,-2"	<< "-1,-1"	<< "2,-2"		<< "1,1";

	while (data.count() >= 8)
	{
		firstF		= StelUtils::strToVec2f(data.takeFirst().toString());
		secondF	= StelUtils::strToVec2f(data.takeFirst().toString());
		vecF		= firstF;
		expected	= data.takeFirst().toBool();
		sumF	= StelUtils::strToVec2f(data.takeFirst().toString());
		diffF		= StelUtils::strToVec2f(data.takeFirst().toString());
		mulF		= StelUtils::strToVec2f(data.takeFirst().toString());
		smF		= StelUtils::strToVec2f(data.takeFirst().toString());
		cwmF	= StelUtils::strToVec2f(data.takeFirst().toString());

		firstD.set((double)firstF[0], (double)firstF[1]);
		vecD		= firstD;
		secondD.set((double)secondF[0], (double)secondF[1]);
		sumD.set((double)sumF[0], (double)sumF[1]);
		diffD.set((double)diffF[0], (double)diffF[1]);
		mulD.set((double)mulF[0], (double)mulF[1]);
		smD.set((double)smF[0], (double)smF[1]);
		cwmD.set((double)cwmF[0], (double)cwmF[1]);

		firstI.set((int)firstF[0], (int)firstF[1]);
		vecI		= firstI;
		secondI.set((int)secondF[0], (int)secondF[1]);
		sumI.set((int)sumF[0], (int)sumF[1]);
		diffI.set((int)diffF[0], (int)diffF[1]);
		mulI.set((int)mulF[0], (int)mulF[1]);
		smI.set((int)smF[0], (int)smF[1]);
		cwmI.set((int)cwmF[0], (int)cwmF[1]);

		result = (firstF==secondF);
		QVERIFY2(result==expected, qPrintable(QString("%1 == %2 (result: %3; expected: %4)")
						      .arg(firstF.toString()).arg(secondF.toString())
						      .arg(result).arg(expected)));
		result = (firstD==secondD);
		QVERIFY2(result==expected, qPrintable(QString("%1 == %2 (result: %3; expected: %4)")
						      .arg(firstD.toString()).arg(secondD.toString())
						      .arg(result).arg(expected)));
		result = (firstI==secondI);
		QVERIFY2(result==expected, qPrintable(QString("%1 == %2 (result: %3; expected: %4)")
						      .arg(firstI.toString()).arg(secondI.toString())
						      .arg(result).arg(expected)));

		result = (firstF!=secondF);
		QVERIFY2(result!=expected, qPrintable(QString("%1 != %2 (result: %3; expected: %4)")
						      .arg(firstF.toString()).arg(secondF.toString())
						      .arg(result).arg(expected)));
		result = (firstD!=secondD);
		QVERIFY2(result!=expected, qPrintable(QString("%1 != %2 (result: %3; expected: %4)")
						      .arg(firstD.toString()).arg(secondD.toString())
						      .arg(result).arg(expected)));
		result = (firstI!=secondI);
		QVERIFY2(result!=expected, qPrintable(QString("%1 != %2 (result: %3; expected: %4)")
						      .arg(firstI.toString()).arg(secondI.toString())
						      .arg(result).arg(expected)));

		Vec2f sumFR = firstF+secondF;
		QVERIFY2(sumFR==sumF, qPrintable(QString("%1 + %2 = %3 (expected: %4)")
						 .arg(firstF.toString())
						 .arg(secondF.toString())
						 .arg(sumFR.toString())
						 .arg(sumF.toString())));
		Vec2d sumDR = firstD+secondD;
		QVERIFY2(sumDR==sumD, qPrintable(QString("%1 + %2 = %3 (expected: %4)")
						 .arg(firstD.toString())
						 .arg(secondD.toString())
						 .arg(sumDR.toString())
						 .arg(sumD.toString())));
		Vec2i sumIR = firstI+secondI;
		QVERIFY2(sumIR==sumI, qPrintable(QString("%1 + %2 = %3 (expected: %4)")
						 .arg(firstI.toString())
						 .arg(secondI.toString())
						 .arg(sumIR.toString())
						 .arg(sumI.toString())));

		Vec2f diffFR = firstF-secondF;
		QVERIFY2(diffFR==diffF, qPrintable(QString("%1 - %2 = %3 (expected: %4)")
						 .arg(firstF.toString())
						 .arg(secondF.toString())
						 .arg(diffFR.toString())
						 .arg(diffF.toString())));

		Vec2d diffDR = firstD-secondD;
		QVERIFY2(diffDR==diffD, qPrintable(QString("%1 - %2 = %3 (expected: %4)")
						 .arg(firstD.toString())
						 .arg(secondD.toString())
						 .arg(diffDR.toString())
						 .arg(diffD.toString())));

		Vec2i diffIR = firstI-secondI;
		QVERIFY2(diffIR==diffI, qPrintable(QString("%1 - %2 = %3 (expected: %4)")
						 .arg(firstI.toString())
						 .arg(secondI.toString())
						 .arg(diffIR.toString())
						 .arg(diffI.toString())));

		Vec2f mulFR = firstF*secondF;
		QVERIFY2(mulFR==mulF, qPrintable(QString("%1 * %2 = %3 (expected: %4)")
						 .arg(firstF.toString())
						 .arg(secondF.toString())
						 .arg(mulFR.toString())
						 .arg(mulF.toString())));

		Vec2d mulDR = firstD*secondD;
		QVERIFY2(mulDR==mulD, qPrintable(QString("%1 * %2 = %3 (expected: %4)")
						 .arg(firstD.toString())
						 .arg(secondD.toString())
						 .arg(mulDR.toString())
						 .arg(mulD.toString())));

		Vec2i mulIR = firstI*secondI;
		QVERIFY2(mulIR==mulI, qPrintable(QString("%1 * %2 = %3 (expected: %4)")
						 .arg(firstI.toString())
						 .arg(secondI.toString())
						 .arg(mulIR.toString())
						 .arg(mulI.toString())));

		Vec2f smFR = firstF*2.f;
		QVERIFY2(smFR==smF, qPrintable(QString("%1 * 2.f = %2 (expected: %3)")
						 .arg(firstF.toString())
						 .arg(smFR.toString())
						 .arg(smF.toString())));

		Vec2d smDR = firstD*2.;
		QVERIFY2(smDR==smD, qPrintable(QString("%1 * 2.0 = %2 (expected: %3)")
						 .arg(firstD.toString())
						 .arg(smDR.toString())
						 .arg(smD.toString())));

		Vec2i smIR = firstI*2;
		QVERIFY2(smIR==smI, qPrintable(QString("%1 * 2 = %2 (expected: %3)")
						 .arg(firstI.toString())
						 .arg(smIR.toString())
						 .arg(smI.toString())));

		vecF *= firstF;
		QVERIFY2(vecF==cwmF, qPrintable(QString("%1 * %1 = %2 (expected: %3)")
						 .arg(firstF.toString())
						 .arg(vecF.toString())
						 .arg(cwmF.toString())));

		vecD *= firstD;
		QVERIFY2(vecD==cwmD, qPrintable(QString("%1 * %1 = %2 (expected: %3)")
						 .arg(firstD.toString())
						 .arg(vecD.toString())
						 .arg(cwmD.toString())));

		vecI *= firstI;
		QVERIFY2(vecI==cwmI, qPrintable(QString("%1 * %1 = %2 (expected: %3)")
						 .arg(firstI.toString())
						 .arg(vecI.toString())
						 .arg(cwmI.toString())));
	}
}

void TestComputations::testVector3Operators()
{
	Vec3f firstF, secondF, vecF, sumF, diffF, smF;
	Vec3d firstD, secondD, vecD, sumD, diffD, smD;
	Vec3i firstI, secondI, vecI, sumI, diffI, smI;
	bool expected, result;
	QVariantList data;
	//           Vec(1)               Vec(2)              Compare   Sum                Diff                   Scal. M.
	data << "0,0,0"	<< "0,0,0"	<< true	<<  "0,0,0"	<<    "0,0,0"	<<   "0,0,0";
	data << "0,0,0"	<< "1,0,0"	<< false	<<  "1,0,0"	<<   "-1,0,0"	<<   "0,0,0";
	data << "0,0,0"	<< "1,1,0"	<< false	<<  "1,1,0"	<< " -1,-1,0"	<<   "0,0,0";
	data << "0,0,0"	<< "1,1,1"	<< false	<<  "1,1,1"	<< "-1,-1,-1"	<<   "0,0,0";
	data << "1,1,1"	<< "0,0,0"	<< false	<<  "1,1,1"	<<    "1,1,1"	<<   "2,2,2";
	data << "1,1,1"	<< "1,1,1"	<< true	<<  "2,2,2"	<<    "0,0,0"	<<   "2,2,2";
	data << "1,-1,0"	<< "1,-1,0"	<< true	<< "2,-2,0"	<<    "0,0,0"	<<  "2,-2,0";
	data << "1,-1,0"	<< "-1,1,0"	<< false	<<  "0,0,0"	<<   "2,-2,0"	<<  "2,-2,0";

	while (data.count() >= 7)
	{
		firstF		= StelUtils::strToVec3f(data.takeFirst().toString());
		secondF	= StelUtils::strToVec3f(data.takeFirst().toString());
		expected	= data.takeFirst().toBool();
		sumF	= StelUtils::strToVec3f(data.takeFirst().toString());
		diffF		= StelUtils::strToVec3f(data.takeFirst().toString());
		smF		= StelUtils::strToVec3f(data.takeFirst().toString());

		firstD	= firstF.toVec3d();
		secondD	= secondF.toVec3d();
		sumD	= sumF.toVec3d();
		diffD		= diffF.toVec3d();
		smD		= smF.toVec3d();

		firstI.set((int)firstF[0], (int)firstF[1], (int)firstF[2]);
		secondI.set((int)secondF[0], (int)secondF[1], (int)secondF[2]);
		sumI.set((int)sumF[0], (int)sumF[1], (int)sumF[2]);
		diffI.set((int)diffF[0], (int)diffF[1], (int)diffF[2]);
		smI.set((int)smF[0], (int)smF[1], (int)smF[2]);

		result = (firstF==secondF);
		QVERIFY2(result==expected, qPrintable(QString("%1 == %2 (result: %3; expected: %4)")
						      .arg(firstF.toString()).arg(secondF.toString())
						      .arg(result).arg(expected)));
		result = (firstD==secondD);
		QVERIFY2(result==expected, qPrintable(QString("%1 == %2 (result: %3; expected: %4)")
						      .arg(firstD.toString()).arg(secondD.toString())
						      .arg(result).arg(expected)));
		result = (firstI==secondI);
		QVERIFY2(result==expected, qPrintable(QString("%1 == %2 (result: %3; expected: %4)")
						      .arg(firstI.toString()).arg(secondI.toString())
						      .arg(result).arg(expected)));

		result = (firstF!=secondF);
		QVERIFY2(result!=expected, qPrintable(QString("%1 != %2 (result: %3; expected: %4)")
						      .arg(firstF.toString()).arg(secondF.toString())
						      .arg(result).arg(expected)));
		result = (firstD!=secondD);
		QVERIFY2(result!=expected, qPrintable(QString("%1 != %2 (result: %3; expected: %4)")
						      .arg(firstD.toString()).arg(secondD.toString())
						      .arg(result).arg(expected)));
		result = (firstI!=secondI);
		QVERIFY2(result!=expected, qPrintable(QString("%1 != %2 (result: %3; expected: %4)")
						      .arg(firstI.toString()).arg(secondI.toString())
						      .arg(result).arg(expected)));

		Vec3f sumFR = firstF+secondF;
		QVERIFY2(sumFR==sumF, qPrintable(QString("%1 + %2 = %3 (expected: %4)")
						 .arg(firstF.toString())
						 .arg(secondF.toString())
						 .arg(sumFR.toString())
						 .arg(sumF.toString())));
		Vec3d sumDR = firstD+secondD;
		QVERIFY2(sumDR==sumD, qPrintable(QString("%1 + %2 = %3 (expected: %4)")
						 .arg(firstD.toString())
						 .arg(secondD.toString())
						 .arg(sumDR.toString())
						 .arg(sumD.toString())));
		Vec3i sumIR = firstI+secondI;
		QVERIFY2(sumIR==sumI, qPrintable(QString("%1 + %2 = %3 (expected: %4)")
						 .arg(firstI.toString())
						 .arg(secondI.toString())
						 .arg(sumIR.toString())
						 .arg(sumI.toString())));

		Vec3f diffFR = firstF-secondF;
		QVERIFY2(diffFR==diffF, qPrintable(QString("%1 - %2 = %3 (expected: %4)")
						 .arg(firstF.toString())
						 .arg(secondF.toString())
						 .arg(diffFR.toString())
						 .arg(diffF.toString())));

		Vec3d diffDR = firstD-secondD;
		QVERIFY2(diffDR==diffD, qPrintable(QString("%1 - %2 = %3 (expected: %4)")
						 .arg(firstD.toString())
						 .arg(secondD.toString())
						 .arg(diffDR.toString())
						 .arg(diffD.toString())));

		Vec3i diffIR = firstI-secondI;
		QVERIFY2(diffIR==diffI, qPrintable(QString("%1 - %2 = %3 (expected: %4)")
						 .arg(firstI.toString())
						 .arg(secondI.toString())
						 .arg(diffIR.toString())
						 .arg(diffI.toString())));

		Vec3f smFR = firstF*2.f;
		QVERIFY2(smFR==smF, qPrintable(QString("%1 * 2.f = %2 (expected: %3)")
						 .arg(firstF.toString())
						 .arg(smFR.toString())
						 .arg(smF.toString())));

		Vec3d smDR = firstD*2.;
		QVERIFY2(smDR==smD, qPrintable(QString("%1 * 2.0 = %2 (expected: %3)")
						 .arg(firstD.toString())
						 .arg(smDR.toString())
						 .arg(smD.toString())));

		Vec3i smIR = firstI*2;
		QVERIFY2(smIR==smI, qPrintable(QString("%1 * 2 = %2 (expected: %3)")
						 .arg(firstI.toString())
						 .arg(smIR.toString())
						 .arg(smI.toString())));
	}
}

void TestComputations::testIntMod()
{
	QVariantList data;

	data << 1 << 1 << 0;
	data << 1 << 2 << 1;
	data << 1 << 3 << 1;
	data << 2 << 4 << 2;
	data << 2 << 5 << 2;

	while (data.count() >= 3)
	{
		int a = data.takeFirst().toInt();
		int b = data.takeFirst().toInt();
		int eR = data.takeFirst().toInt();
		int r = StelUtils::imod(a, b);

		QVERIFY2(r==eR, qPrintable(QString("%1 mod %2 = %3 (expected %4)")
					   .arg(a)
					   .arg(b)
					   .arg(r)
					   .arg(eR)));
	}
}

void TestComputations::testFloatMod()
{
	QVariantList data;

	data << 1 << 1 << 0;
	data << 1 << 2 << 1;
	data << 1 << 3 << 1;
	data << 2 << 4 << 2;
	data << 2 << 5 << 2;

	while (data.count() >= 3)
	{
		float a = data.takeFirst().toFloat();
		float b = data.takeFirst().toFloat();
		float eR = data.takeFirst().toFloat();
		float r = StelUtils::fmodpos(a, b);

		QVERIFY2(qAbs(r-eR)<=1e-5, qPrintable(QString("%1 mod %2 = %3 (expected %4)")
					   .arg(QString::number(a, 'f', 2))
					   .arg(QString::number(b, 'f', 2))
					   .arg(QString::number(r, 'f', 2))
					   .arg(QString::number(eR, 'f', 2))));
	}
}

void TestComputations::testDoubleMod()
{
	QVariantList data;

	data << 1 << 1 << 0;
	data << 1 << 2 << 1;
	data << 1 << 3 << 1;
	data << 2 << 4 << 2;
	data << 2 << 5 << 2;

	while (data.count() >= 3)
	{
		double a = data.takeFirst().toDouble();
		double b = data.takeFirst().toDouble();
		double eR = data.takeFirst().toDouble();
		double r = StelUtils::fmodpos(a, b);

		QVERIFY2(qAbs(r-eR)<=1e-5, qPrintable(QString("%1 mod %2 = %3 (expected %4)")
					   .arg(QString::number(a, 'f', 2))
					   .arg(QString::number(b, 'f', 2))
					   .arg(QString::number(r, 'f', 2))
					   .arg(QString::number(eR, 'f', 2))));
	}
}
