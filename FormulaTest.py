import matplotlib.pyplot as plt
import numpy as np

def main():
    angleList = np.arange(1, 90, 10)
    numVals = angleList.size
    xValues = np.array(numVals)
    yValues = np.array(numVals)
    turnWheelVal1 = np.array(numVals)
    turnWheelVal2 = np.array(numVals)
    lvr = np.array(numVals)
    xvy = np.array(numVals)
    xValues = np.round(np.cos(np.radians(angleList)), 4)
    xValues *= 100
    xValues.fill(50)
    yValues = np.round(np.sin(np.radians(angleList)), 4)
    yValues *= 100
    xvy = xValues/yValues
    turnWheelVal1 = yValues - (yValues*((xValues)/100))
    turnWheelVal2 = yValues - (np.power(xValues, 2)*0.01) 

    lvr1 = turnWheelVal1/yValues
    lvr2 = turnWheelVal2/yValues
   
    for i in range(numVals):
        print("- - - - - - - - - - - - - - - - \n")
        print("X: " + str(round(xValues[i], 5)) + "   Y: " + str(round(yValues[i], 5)) + "\n")
        print("Formula 1 = LW: " + str(round(yValues[i], 5)) + "   RW: " + str(round(turnWheelVal1[i], 5)) + "\n")
        print("Formula 2 = LW: " + str(round(yValues[i], 5)) + "   RW: " + str(round(turnWheelVal2[i], 5)) + "\n")
        print("Left vs Right Ratio 1 = " + str(round(lvr1[i], 5)))
        print("Left vs Right Ratio 2 = " + str(round(lvr2[i], 5)))
        print("X vs Y Ratio = " + str(round(xvy[i], 5)))
    plt.plot(xvy, lvr1)
    plt.plot(xvy, lvr2)
    
    plt.xlabel('X vs Y values')
    plt.ylabel('Left vs Right')

    plt.show()

main()