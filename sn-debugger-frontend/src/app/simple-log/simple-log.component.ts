import { Component, ElementRef, ViewChild } from '@angular/core';
import { HttpClient } from '@angular/common/http';

@Component({
  selector: 'app-simple-log',
  templateUrl: './simple-log.component.html',
  standalone: false
})
export class SimpleLogComponent {
  @ViewChild('logid') logEl!: ElementRef<HTMLDetailsElement>;

  threadnum: number = 0;            // Input later if needed
  currentstepcount: number = 0;     // Sync this from parent later
  logstepcount: number = 0;

  maxlog: number = 100;
  buflog: number = 20;
  startlog: number = 0;
  displayedlogentries: number = 0;
  countlogentries: number = 0;

  logs: any[] = [];

  constructor(private http: HttpClient) {}

  loadlog(opening: boolean): void {
    const field = this.logEl?.nativeElement;

    if (this.logstepcount !== this.currentstepcount) {
      this.logstepcount = this.currentstepcount;
      this.startlog = 0;
      this.displayedlogentries = 0;
    }

    if ((opening && !field?.open) || (!opening && field?.open)) {
      if (this.startlog < this.maxlog && this.startlog < this.countlogentries) {
        const url = `http://127.0.0.1/log.json?threadnum=${this.threadnum}&maxlogentries=${this.buflog}&startlog=${this.startlog}&stepcount=${this.logstepcount}`;

        this.http.get<any>(url).subscribe(response => {
          if (this.logstepcount === this.currentstepcount && this.logstepcount === response.stepcount) {
            const records = response.records || [];

            if (this.startlog === 0) {
              this.logs = records;
              this.displayedlogentries = 0;
            } else {
              this.logs.push(...records);
            }

            this.displayedlogentries += records.length;
            this.startlog += this.buflog;

            this.countlogentries = response.countlogentries ?? this.countlogentries;

            if (this.startlog < this.maxlog && this.startlog < this.countlogentries) {
              this.loadlog(false);
            }
          }
        });
      }
    }
  }

  trustAsHtml(value: string): string {
    return value;
  }
}
