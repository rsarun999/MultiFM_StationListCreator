using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using IniParser;
using IniParser.Model;

//https://sourceforge.net/projects/nsis/ for source repacking
//"dotnet add package ini-parser --version 2.5.2"  & for more in https://github.com/rickyah/ini-parser
// dotnet add package System.Linq --version 4.3.0 
// dotnet add package System.Globalization --version 4.3.0
//to create an *.exe file execute dotnet publish -c release -r win10-x64

namespace MultiFM_StationCreator {
    static class Constants {
        //public const double start_freq = 100.0;
        public const double end_freq = 108.0;
        //public const int stations2create = 40;
        //public const string region = "GB";
        public const double freq_increment = 0.6;
    }
    class Program {
        static void Main (string[] args) {
            //lists to store Country code, PI code & Programme Service name from Lookup table.
            List<string> list_CC = new List<string> ();
            List<string> list_ECC = new List<string> ();
            List<string> list_PI = new List<string> ();
            List<string> list_PSN = new List<string> ();

            //filtered PI & PSN containing only information related to country selected by the user.
            List<string> firstFewPI = new List<string> ();
            List<string> firstFewPSN = new List<string> ();
            List<string> firstFewECC = new List<string> ();

            /*
            foreach (var a in args) {
                Console.WriteLine ("Arguments to print are: ", a + System.Environment.NewLine);
            }*/

            double start_freq = Convert.ToDouble (args[0]);
            int stations2create = Int32.Parse (args[1]);
            string region = args[2];

            //lists to hold 34 frequencies from 88-107.8 MHz.
            List<string> list_random_FMfreq = new List<string> ();
            int i_count = 0;

            //for loop runs until the end of frequency is reached and then is reset back to start.
            //use mod operator "%" if needed to create a station of more than 34 list size.
            for (double i_loop = start_freq; i_loop <= Constants.end_freq; i_loop += Constants.freq_increment) {
                //convert double var to single decimal point var and copy to string of lists.                
                i_loop = Math.Round (i_loop, 1);
                string str = i_loop.ToString (CultureInfo.GetCultureInfo ("en-GB"));
                list_random_FMfreq.Add (str);
                i_count++;
            }

            if (i_count < stations2create) {
                for (double i_loop = start_freq; i_count <= stations2create; i_loop += Constants.freq_increment) {
                    //convert double var to single decimal point var and copy to string of lists.
                    if (i_loop >= Constants.end_freq) {
                        i_loop = start_freq;
                    }
                    i_loop = Math.Round (i_loop, 1);
                    string str = i_loop.ToString (CultureInfo.GetCultureInfo ("en-GB"));
                    list_random_FMfreq.Add (str);
                    i_count++;
                }
            }

            using (var reader = new StreamReader (@"LUT.csv")) {
                //read 1 time outside the while loop to skip reading the heading row !
                reader.ReadLine ();
                while (!reader.EndOfStream) {
                    var line = reader.ReadLine ();
                    var values = line.Split (',');

                    list_CC.Add (values[0]);
                    list_ECC.Add (values[1]);
                    list_PI.Add (values[2]);
                    list_PSN.Add (values[3]);
                }

                //disabled routine to extract the distinct countries used in LUT table and construct windows form.

                // Get distinct Country Code and convert into a list again.
                List<string> list_Distinct_CC = new List<string> ();
                /* 
                //lists to be available as input to the user in drop-down box.
                list_Distinct_CC = list_CC.Distinct().ToList();
            
                foreach(var item in list_Distinct_CC)
                {        
                    string pre = "\"";
                    string post = "\",";
                    string merge = string.Concat(pre,item,post);

                    Console.WriteLine(item);
                } */
            }
            var search_CC_List = list_CC
                .Select ((v, i) => new { Index = i, Value = v })
                .Where (x => x.Value == region)
                .Select (x => x.Index)
                .ToList ();

            //skip few random stations within first 50 items and take as many stations as requested by the user.
            Random rnd = new Random ();
            foreach (var item in search_CC_List.Skip (rnd.Next (1, 50)).Take (stations2create)) {
                firstFewECC.Add (list_ECC[item]);
                firstFewPI.Add (list_PI[item]);
                firstFewPSN.Add (list_PSN[item]);
            }

            string builder_channel = File.ReadAllText (@"builder_channel.txt", Encoding.UTF8);
            string builder_rds = File.ReadAllText (@"builder_rds.txt", Encoding.UTF8);
            string audio_file = File.ReadAllText (@"audio_playlist.txt", Encoding.UTF8);

            var parser = new FileIniDataParser ();
            string fileName = string.Concat("parser_",DateTime.Now.ToString("hhmmss")+".ini");
            
            using (File.Create(fileName)) {}
            
            IniData data = parser.ReadFile (fileName);

            foreach (var i in Enumerable.Range (1, stations2create)) {
                string channel = String.Concat ("channel.", i);
                data.Sections.AddSection (channel);

                string rds = String.Concat ("rds.", i);
                data.Sections.AddSection (rds);
            }

            //routine to append channel & rds parameter text to parser ini file
            using (File.Create(@"merge_content.ini")) {}
            using (var write = File.CreateText (@"merge_content.ini")) {
                write.WriteLine (audio_file);
                foreach (var i in Enumerable.Range (1, stations2create)) {
                    string channel = String.Concat ("[channel.", i, "]");
                    string rds = String.Concat ("[rds.", i, "]");

                    write.WriteLine (channel);
                    write.WriteLine (builder_channel);

                    write.WriteLine (rds);
                    write.WriteLine (builder_rds);
                }
            }

            foreach (var i in Enumerable.Range (1, stations2create)) {
                string channel = String.Concat ("channel.", i);
                string rds = String.Concat ("rds.", i);

                data[channel]["freq"] = list_random_FMfreq[i - 1];
                data[channel]["idrds"] = i.ToString ();

                data[rds]["name"] = String.Concat ("RDS_", i);
                long Dec = Int32.Parse (firstFewECC[i - 1], System.Globalization.NumberStyles.HexNumber);
                string DecString = String.Format ("{0:0}", Dec);
                data[rds]["slow"] = DecString;
                data[rds]["ps"] = firstFewPSN[i - 1];
                data[rds]["pi"] = firstFewPI[i - 1];
            }

            var merge_parser = new IniParser.Parser.IniDataParser ();

            IniData user_config = merge_parser.Parse (File.ReadAllText ("merge_content.ini"));
            data.Merge (user_config);

            //Save the file
            parser.WriteFile (fileName, data);
        }
    }
}