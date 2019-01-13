import matplotlib.pyplot as plt
import numpy as np

def main():
    angleList = np.arange(1, 90)
    numVals = angleList.size
    xValues = np.array(numVals)
    yValues = np.array(numVals)
    turnWheelVal = np.array(numVals)
    lvr = np.array(numVals)
    xvy = np.array(numVals)
    xValues = np.round(np.cos(np.radians(angleList)), 4)
    xValues *= 100
    xValues.fill(10)
    yValues = np.round(np.sin(np.radians(angleList)), 4)
    yValues *= 100
    xvy = yValues/xValues
    turnWheelVal = yValues - (yValues*(((xValues)/100)))
    #turnWheelVal = yValues - (xValues*(((yValues)/100)))

    lvr = yValues/turnWheelVal
    for i in range(numVals):
        print("- - - - - - - - - - - - - - - - \n")
        print("X: " + str(xValues[i]) + "   Y: " + str(yValues[i]) + "\n")
        print("LW: " + str(yValues[i]) + "   RW: " + str(turnWheelVal[i]) + "\n")
        print("Left vs Right Ratio = " + str(lvr[i]))
        print("X vs Y Ratio = " + str(xvy[i]))
    plt.plot(lvr, yValues)
    plt.ylabel('Y values')
    plt.xlabel('Left vs Right')

    plt.show()

main()