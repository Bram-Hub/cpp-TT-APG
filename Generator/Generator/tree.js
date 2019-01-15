var simple_chart_config = {
	chart: {
		container: "#OrganiseChart-simple"
	},

	nodeStructure: {
innerHTML: "<div style=\"text-align:center\"><p>A &#8594; B<br>B &#8594; C<br>A<br>C</p></div>",
children: [
{
innerHTML: "<div style=\"text-align:center\"><p>&#172;A<br><span style=\"color: #01DF01\">&#10005;</span></p></div>"
},
{
innerHTML: "<div style=\"text-align:center\"><p>B</p></div>",
children: [
{
innerHTML: "<div style=\"text-align:center\"><p>&#172;B<br><span style=\"color: #01DF01\">&#10005;</span></p></div>"
},
{
innerHTML: "<div style=\"text-align:center\"><p>C<br><span style=\"color: #ff0000\"><b>&#9711;</b></span></p></div>"
}
]
}
]
}
};