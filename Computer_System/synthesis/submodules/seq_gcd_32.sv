module seq_gcd_32(
   input csi_clk,
   input rsi_reset_n,
   input avs_s0_write,
   input avs_s1_write,
   input avs_s2_read,
   input avs_s3_read,
   input [31:0] avs_s0_writedata,     //input A
   input [31:0] avs_s1_writedata,     //input B
   output reg [31:0] avs_s2_readdata, //output result
   output reg [7:0] avs_s3_readdata   //output ready (valid bit)
);
	//registers for A and B operands
   reg [31:0] a;
   reg [31:0] b;
	
	//registers to store input state
   reg a_loaded = 0;
   reg b_loaded = 0;
	
	//start module reporting no valid output to memory mapped interface
   initial begin
      avs_s3_readdata = 0;
   end

   always @(posedge csi_clk) begin
		//if A has been written to then store the result in register and remember it was loaded
      if (avs_s0_write) begin
         a <= avs_s0_writedata;
         a_loaded <= 1;
      end
		//if b has been written to then store the result in register and remember it was loaded
      else if (avs_s1_write) begin
         b <= avs_s1_writedata;
         b_loaded <= 1;
      end
		//if a and b are already loaded begin calculation
      else if(a_loaded && b_loaded) begin
			//euclid's algorithm
         if (a > b) begin
            a <= a - b;
         end
         else if (b > a) begin
            b <= b - a;
         end
			//if a result has been reached, then output the data, set the valid bit, and reset the input data valid registers
         else if (a == b && a != 0) begin
            avs_s2_readdata <= a;
            avs_s3_readdata <= 1;
            a_loaded <= 0;
            b_loaded <= 0;
         end
      end
		//if the output has been read from, then reset the valid bit
      else if(avs_s2_read) begin
         avs_s3_readdata <= 0;
      end
   end
endmodule
