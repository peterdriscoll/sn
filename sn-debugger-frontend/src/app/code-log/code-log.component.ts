import { Component, Input, OnChanges, SimpleChanges } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { DomSanitizer, SafeHtml } from '@angular/platform-browser';

@Component({
  selector: 'app-code-log',
  templateUrl: './code-log.component.html',
  standalone: false
})
export class CodeLogComponent implements OnChanges {
  @Input() threadnum = 0;
  @Input() currentstepcount = 0;
  @Input() countcodeentries = 0;

  code: any[] = [];
  displayedcode = 0;

  private codestepcount = 0;
  private startcode = 0;
  private readonly maxcode = 30;
  private readonly bufcode = 10;

  constructor(private http: HttpClient, private sanitizer: DomSanitizer) {}

  ngOnChanges(changes: SimpleChanges): void {
    if (changes['currentstepcount'] && !changes['currentstepcount'].firstChange) {
      this.codestepcount = this.currentstepcount;
      this.startcode = 0;
      this.displayedcode = 0;
    }
  }

  loadcode(opening: boolean): void {
    if (this.codestepcount !== this.currentstepcount) {
      this.codestepcount = this.currentstepcount;
      this.startcode = 0;
      this.displayedcode = 0;
    }

    if (this.startcode < this.maxcode && this.startcode < this.countcodeentries) {
      const url = `http://127.0.0.1/code.json?threadnum=${this.threadnum}&maxcode=${this.bufcode}&startcode=${this.startcode}&stepcount=${this.codestepcount}`;
      this.http.get<any>(url).subscribe(response => {
        if (response.stepcount === this.codestepcount) {
          const records = response.records || [];
          if (this.startcode === 0) {
            this.code = records;
            this.displayedcode = 0;
          } else {
            this.code.push(...records);
          }

          this.displayedcode += records.length;
          this.startcode += this.bufcode;

          if (this.startcode < this.maxcode && this.startcode < this.countcodeentries) {
            this.loadcode(false);
          }
        }
      });
    }
  }

  trustAsHtml(html: string): SafeHtml {
    return this.sanitizer.bypassSecurityTrustHtml(html);
  }
}
