let in_chan = open_in "q12.txt" in
  let rec line_consumer acc capture_state shapes regions = 
    match input_line in_chan with
    | line -> 
      let check1 = String.contains line ':' in
      let check2 = String.contains line 'x' in
      let cap_lines = check1 && check2 in
      if cap_lines then
        line_consumer acc capture_state shapes (Array.append regions [|line|])
      else
      let cap_shapes = check1 && not check2 in
      let flush_buff = line = "" in
      let is_capturing = 
        if flush_buff then
          false
        else if capture_state then
          capture_state
        else if cap_shapes then
          cap_shapes
        else 
          false
      in
      if is_capturing && cap_shapes then
        line_consumer (Printf.sprintf "%s" line) is_capturing shapes regions
      else if is_capturing then
        line_consumer (Printf.sprintf "%s\n%s" acc line) is_capturing shapes regions
      else
        let start_idx = String.index acc ':' + 2 in
        let n = String.length acc in
        let shape = String.sub acc start_idx (n-start_idx) in
        let new_shapes = Array.append shapes [|shape|] in
        line_consumer "" is_capturing new_shapes regions;
    | exception End_of_file ->
        (shapes,regions)
    in
  let shapes, regions = line_consumer "" false [||] [||] in
  let exact_areas = Array.map (fun s -> String.fold_left (fun acc x -> acc + if x = '#' then 1 else 0) 0 s) shapes in
  (* Array.iter (fun s -> Printf.printf "%s\n\n" s) shapes;
  Array.iter (fun s -> Printf.printf "%s\n" s) regions;
  Array.iter (Printf.printf "%d ") exact_areas; print_newline(); *)
  
  let sum arr = Array.fold_left (fun acc x -> acc + x) 0 arr in

  let solve_single_region region = 
    (* Parsing region string *)
    let n = String.length region in
    let col_idx = String.index region ':' in
    let dims = String.sub region 0 col_idx |> String.split_on_char 'x' |> Array.of_list |> Array.map int_of_string in
    let shapes_used = String.sub region ( col_idx+2 ) (n-col_idx-2) |> String.split_on_char ' ' |> Array.of_list |> Array.map int_of_string  in
    
    let rect_area = dims.(0) * dims.(1) in 
    
    let maximised_shape_areas = Array.map (fun x -> x * 9) shapes_used |> sum in
    let minimised_shape_areas = Array.map2 (fun x y -> x * y ) shapes_used exact_areas |> sum in

    if maximised_shape_areas <= rect_area then
      1
    else if minimised_shape_areas > rect_area then
      0
    else
      assert false;
  in
  let ans = Array.fold_left (fun acc x -> acc + solve_single_region x) 0 regions in
  Printf.printf "%d\n" ans
      