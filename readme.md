# TT Generator
## Authors
2018
Troy Ferrazzano
Jasper Boland

## Usage
### Technical installation/Requirements
This program was compiled with Visual Studio. For it to work, you must have
VC++ Runtime libraries installed. VC++ is included with a Visual Studio download.
If you do not have Visual Studio or VC++ Runtime libraries installed,
here is the download link for VC++ Runtime libraries (15 mb download):

https://www.microsoft.com/en-us/download/details.aspx?id=30679
---------------------------------------------------------------------------------

### Running the program
Open the 'Generator' file found in the same directory as this INSTRUCTIONS.txt.
Open 'Debug' file.
There will be an application called 'Generator'. Simply double click it.
You will be asked how many premises the argument has. This number includes the conclusion.
Next, you will be asked to enter in the premises and conclusion. Enter each premise one at a time, 
pressing enter after each premise. 

Key for entering symbols:
Not    Or    And    Implication    Biconditional
 ~     |      &	       ->              <->

Example:
'Enter the number of premises you have:'
3
'Enter your premises now:'
P->Q
P
~Q (Don't forget to negate your conclusion!)

The generator will now solve the tree. 
All you now need to do is open the HTML file called 'TruthTree', found in the same directory as 'Generator'.
Upon opening 'TruthTree', you will see the validity of your argument. 

All green X's - valid argument.
A red O - invalid argument.

Simply run 'Generator' again to test the validity of a different argument.

