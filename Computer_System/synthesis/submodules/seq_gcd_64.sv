module seq_gcd_64(
   input csi_clk,
   input rsi_reset_n,
   input avs_s0_write,
   input avs_s1_write,
   input avs_s2_write,
   input avs_s3_write,
   input avs_s4_read,
   input avs_s5_read,
   input [31:0] avs_s0_writedata, //input a high bits
   input [31:0] avs_s1_writedata, //input a low bits
   input [31:0] avs_s2_writedata, //input b high bits
   input [31:0] avs_s3_writedata, //input bt low bits
   output reg [63:0] avs_s4_readdata, //output data
   output reg [7:0] avs_s5_readdata //output ready (valid bit)

);
	//registers to store a and b
   reg [63:0] a = 0;
   reg [63:0] b = 0;
	
	//registers to store input state
   reg a_hi_loaded = 0;
   reg a_lo_loaded = 0;
   reg b_hi_loaded = 0;
   reg b_lo_loaded = 0;
	
	//start by reporting no valid output and setting output to zero
   initial begin
      avs_s5_readdata = 0;
      avs_s4_readdata = 0;
   end
	
	//on every clock cycle
   always @(posedge csi_clk) begin
		//if a high has been written to, store it and set its input state register to 1
      if (avs_s0_write) begin
         a[63:32] <= avs_s0_writedata;
         a_hi_loaded <= 1;
      end
		//if a low has been written to, store it and set its input state register to 1
      else if (avs_s1_write) begin
         a[31:0] <= avs_s1_writedata;
         a_lo_loaded <= 1;
      end
		//if b high has been written to, store it and set its input state register to 1
      else if (avs_s2_write) begin
         b[63:32] <= avs_s2_writedata;
         b_hi_loaded <= 1;
      end
		//if b low has been written to, store it and set its input state register to 1
      else if (avs_s3_write) begin
         b[31:0] <= avs_s3_writedata;
         b_lo_loaded <= 1;
      end
		//if all have been loaded begin calculation
      else if(a_hi_loaded && a_lo_loaded && b_hi_loaded && b_lo_loaded) begin
			//standard euclid's algorithm on 64 bit operands
         if (a > b) begin
            a <= a - b;
         end
         else if (b > a) begin
            b <= b - a;
         end
			//if the result is reached
         else if (a == b) begin
				//set the output, set the output valid bit, reset the input valid bit
            avs_s4_readdata = a;
            avs_s5_readdata = 1;
            a_hi_loaded <= 0;
            a_lo_loaded <= 0;
            b_hi_loaded <= 0;
            b_lo_loaded <= 0;
         end
      end
		//if the output has been read, then reset the output valid bit
      else if(avs_s4_read) begin
         avs_s5_readdata <= 0;
      end
   end
endmodule
