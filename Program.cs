using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using IniParser;
using IniParser.Model;
using System.Linq;
using System.Globalization;

//https://sourceforge.net/projects/nsis/ for source repacking
//"dotnet add package ini-parser --version 2.5.2"  & for more in https://github.com/rickyah/ini-parser
// dotnet add package System.Linq --version 4.3.0 
// dotnet add package System.Globalization --version 4.3.0

namespace MultiFM_StationCreator
{
    class Program
    {
        static void Main(string[] args)
        {
            //lists to store Country code, PI code & Programme Service name from Lookup table.
            List<string> list_CC = new List<string>();
            List<string> list_PI = new List<string>();
            List<string> list_PSN = new List<string>();

            //filtered PI & PSN containing only information related to country selected by the user.
            List<string> firstFewPI = new List<string>();
            List<string> firstFewPSN = new List<string>();

            //lists to hold 34 frequencies from 88-107.8 MHz.
            //use mod operator % if a station of more than 34 list size is needed.
            List<string> list_random_FMfreq = new List<string>();

            for (double i = 88; i <= 108; i += 0.6)
            {
                //convert double var to single decimal point var and copy to string of lists.
                i = Math.Round(i, 1);
                string str = i.ToString(CultureInfo.GetCultureInfo("en-GB"));
                list_random_FMfreq.Add(str);
            }

            using (var reader = new StreamReader(@"LUT.csv"))
            {
                //read 1 time outside the while loop to skip reading the heading row !
                reader.ReadLine();
                while (!reader.EndOfStream)
                {
                    var line = reader.ReadLine();
                    var values = line.Split(',');

                    list_CC.Add(values[0]);
                    list_PI.Add(values[2]);
                    list_PSN.Add(values[3]);
                }

                //disabled routine to extract the distinct countries used in LUT table and construct windows form.
                 
                // Get distinct Country Code and convert into a list again.
                List<string> list_Distinct_CC = new List<string>();

                //lists to be available as input to the user in drop-down box.
                list_Distinct_CC = list_CC.Distinct().ToList();
            
                foreach(var item in list_Distinct_CC)
                {        
                    string pre = "\"";
                    string post = "\",";
                    string merge = string.Concat(pre,item,post);

                    Console.WriteLine(item);
                } 
            }
            var search_CC_List = list_CC
                     .Select((v, i) => new { Index = i, Value = v })
                     .Where(x => x.Value == "DE")
                     .Select(x => x.Index)
                     .ToList();

            foreach (var item in search_CC_List.Take(10))
            {
                firstFewPI.Add(list_PI[item]);
                firstFewPSN.Add(list_PSN[item]);
            }

            string builder_channel = File.ReadAllText(@"builder_channel.txt", Encoding.UTF8);
            string builder_rds = File.ReadAllText(@"builder_rds.txt", Encoding.UTF8);

            var parser = new FileIniDataParser();
            IniData data = parser.ReadFile("parser.ini");

            foreach (var i in Enumerable.Range(1, 10))
            {
                string channel = String.Concat("channel.", i);
                data.Sections.AddSection(channel);

                string rds = String.Concat("rds.", i);
                data.Sections.AddSection(rds);
            }

            //routine to append channel & rds parameter text to parser ini file
            using (var write = File.CreateText(@"merge_content.ini"))
            {
                foreach (var i in Enumerable.Range(1, 10))
                {
                    string channel = String.Concat("[channel.", i, "]");
                    string rds = String.Concat("[rds.", i, "]");

                    write.WriteLine(channel);
                    write.WriteLine(builder_channel);

                    write.WriteLine(rds);
                    write.WriteLine(builder_rds);
                }
            }

            foreach (var i in Enumerable.Range(1, 10))
            {
                string channel = String.Concat("channel.", i);
                string rds = String.Concat("rds.", i);

                data[channel]["freq"] = list_random_FMfreq[i - 1];
                data[channel]["idrds"] = i.ToString();

                data[rds]["name"] = String.Concat("RDS_", i);
                data[rds]["ps"] = firstFewPSN[i - 1];
                data[rds]["pi"] = firstFewPI[i - 1];
            }
            //Save the file
            parser.WriteFile("parser.ini", data);

            var merge_parser = new IniParser.Parser.IniDataParser();

            IniData user_config = merge_parser.Parse(File.ReadAllText("merge_content.ini"));
            data.Merge(user_config);
        }
    }
}