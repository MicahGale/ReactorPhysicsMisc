%%%22.212 Fall 2017- Written by Micah Gale

%A class for handling \LaTeX tables

classdef texTable
    properties%( Access=private)
        headers  %the header column names
        content %the actual cell contents stored as cell array
        everyLine %do you want \hline every line?
        format    %the {|c|c|} formatting
        
    end
    
    methods
        %Builds a textable objec
%         
%         Inputs:
%         headers: a row matrix of the column headers for the table
%         everyLine: boolean: do you want \hline used
%         format: the table formatting section. eg {|c|c}
        function this=texTable(headers,everyLine,format)
            big=size(headers);
            if big(1) == 1 %check that is a row vector
                this.headers=headers;
            else
                disp('bad header dimensions');
            end
            this.everyLine=everyLine;
            this.format=format;
            this.content={};
        end
        
%         Adds data to the meat of the table.
%         
%         Inputs: data- an array of the data. The number of rows must match
%         the headers. data must be a cell array of strings
%         
        function this=addData(this, data)
            big=size(data);
            if big(2) == length(this.headers) %checks to make sure that format matches headers
                this.content=[this.content; data];
            else
                disp('number of columns is wrong');
            end
        end
        
        %Writes the table to a file in tex formatting
        
        %Inputs: 
        %target: a string referring to the file name to be written to
        function writeTable(this,target)
            f=fopen(target,'w'); %opens target file
            fprintf(f,'\\begin{tabular}%s \n',this.format); %opens the table
            if this.everyLine
                fprintf(f,'\\hline \n');
            end
            for i=1:length(this.headers) % print out all of the headers
                if i < length(this.headers) % checks if last column
                    fprintf(f,'\\textbf{%s} & ',this.headers{i});
                else
                    fprintf(f,'\\textbf{%s} \\\\ \n',this.headers{i});
                    if this.everyLine
                        fprintf(f,'\\hline \n'); %add in the hline
                    end
                end  
            end
                big=size(this.content);
                for i=1:big(1)   %loop through rows
                    for j=1:big(2) %loop through columns
                        if j==big(2)
                            fprintf(f,' %s \\\\ \n',this.content{i,j});
                            if this.everyLine
                                fprintf(f,'\\hline \n');
                            end
                        else
                            fprintf(f,' %s & ',this.content{i,j}); %print out the content
                        end
                    end
                end
            fprintf(f,'\\end{tabular}'); %%%End the table
            fclose(f);
        end
    end
end
