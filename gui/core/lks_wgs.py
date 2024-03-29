#coding: utf-8
import math
#konversija iš excel failo
#http://www.geocad.lt/lt/excel-skaiciuokles/7-coordinate-conversion-lks-94-to-wgs-84
#baisesnio užrašo nesu matęs :)


def lks94_to_wgs(LKS_x, LKS_y):
    B2 = LKS_y
    B3 = LKS_x

    G32 = 6378137.000
    G33 = 6356752.314
    G34 = 0.9998
    G35 = 500000.000
    G36 = 0.000
    G37 = G32 * G34
    G38 = G33 * G34
    L30 = 0.0
    L31 = 0.4188790205

    G18 = (G37-G38)*1.0/(G37+G38)
    G19 = ((B2-G36)*1.0/G37)+L30
    G20 = G38*(((1+G18+((5.0/4)*(G18**2))+((5.0/4)*(G18**3)))*(G19-L30))-(((3*G18)+(3*(G18**2))+((21.0/8)*(G18**3)))*(math.sin(G19-L30))*(math.cos(G19+L30)))+((((15.0/8)*(G18**2))+((15.0/8)*(G18**3)))*(math.sin(2*(G19-L30)))*(math.cos(2*(G19+L30))))-(((35.0/24)*(G18**3))*(math.sin(3*(G19-L30)))*(math.cos(3*(G19+L30)))))
    #G21=((B2-G36-G20)*1.0/G37)+G19
    G22=((B2-G36-G20)*1.0/G37)+G19
    G23= G38*(((1+G18+((5.0/4)*(G18**2))+((5.0/4)*(G18**3)))*(G22-L30))-(((3*G18)+(3*(G18**2))+((21.0/8)*(G18**3)))*(math.sin(G22-L30))*(math.cos(G22+L30)))+((((15.0/8)*(G18**2))+((15.0/8)*(G18**3)))*(math.sin(2*(G22-L30)))*(math.cos(2*(G22+L30))))-(((35.0/24)*(G18**3))*(math.sin(3*(G22-L30)))*(math.cos(3*(G22+L30)))))
    G24=G22
    G25 =((B2-G36-G23) * 1.0/G37)+G24
    G27 = G25
    L4 = G27
    G5 = (G37**2-G38**2)/G37**2.0
    G6 = G37/(math.sqrt(1-(G5*(math.sin(L4))**2)))
    G7 =(G6*(1-G5))/(1-(G5*(math.sin(L4))**2))
    G8 = ((1.0*G6)/G7)-1
    G9 = G38*(((1+G18+((5.0/4)*(G18**2))+((5.0/4)*(G18**3)))*(L4-L30))-(((3*G18)+(3*(G18**2))+((21.0/8)*(G18**3)))*(math.sin(L4-L30))*(math.cos(L4+L30)))+((((15.0/8)*(G18**2))+((15.0/8)*(G18**3)))*(math.sin(2*(L4-L30)))*(math.cos(2*(L4+L30))))-(((35.0/24)*(G18**3))*(math.sin(3*(L4-L30)))*(math.cos(3*(L4+L30)))))

    #G26 =G38*(((1+G18+((5.0/4)*(G18**2))+((5.0/4)*(G18**3)))*(G25-L30))-(((3*G18)+(3*(G18**2))+((21.0/8)*(G18**3)))*(math.sin(G25-L30))*(math.cos(G25+L30)))+((((15.0/8)*(G18**2))+((15.0/8)*(G18**3)))*(math.sin(2*(G25-L30)))*(math.cos(2*(G25+L30))))-(((35.0/24)*(G18**3))*(math.sin(3*(G25-L30)))*(math.cos(3*(G25+L30)))))

    G10 = B3-G35
    G11 = (math.tan(L4))/(2.0*G6*G7)
    G12 = ((math.tan(L4))/(24.0*G7*G6**3))*(5+(3*((math.tan(L4))**2))+G8-(9*((math.tan(L4))**2)*G8))
    G13 =((math.tan(L4))/(720.0*G7*G6**5))*(61+(90*((math.tan(L4))**2))+(45*((math.tan(L4))**4)))


    G14 = ((math.cos(L4))**-1)/G6
    G15 =(((math.cos(L4))**-1)/(6*G6**3))*(((1.0*G6)/G7)+(2*((math.tan(L4))**2)))
    G16 =(((math.cos(L4))**-1)/(120*G6**5))*(5+(28*((math.tan(L4))**2))+(24*((math.tan(L4))**4)))
    G17 = (((math.cos(L4))**-1)/(5040*G6**7))*(61+(662*((math.tan(L4))**2))+(1320*((math.tan(L4))**4))+(720*((math.tan(L4))**6)))


    #I4 = math.floor((math.abs(K4)-H4)*60)
    #J4 = ((math.abs(K4))*3600)-(I4*60)-(H4*3600)
    #K4 = DEGREES(L4)
    rad_lat = (L4-((G10**2)*G11)+((G10**4)*G12)-((G10**6)*G13))
    rad_lon = L31+(G10*G14)-((G10**3)*G15)+((G10**5)*G16)-((G10**7)*G17)  

    lat = 180 * rad_lat/math.pi
    lon = 180 * rad_lon/math.pi
    return lat,lon
