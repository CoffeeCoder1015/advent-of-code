let in_chan = open_in "test.txt" in
let rec line_consumer _ =
   match input_line in_chan with
    | line -> 
      print_endline line;
      line_consumer ()
    | exception End_of_file -> 
      close_in in_chan;
    in
  line_consumer ()
;; 

