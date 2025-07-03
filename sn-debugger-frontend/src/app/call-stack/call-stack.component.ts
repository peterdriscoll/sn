import { Component, Input, OnChanges, SimpleChanges, ElementRef, ViewChild } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { DomSanitizer, SafeHtml } from '@angular/platform-browser';

@Component({
  selector: 'app-call-stack',
  templateUrl: './call-stack.component.html',
  standalone: false
})
export class CallStackComponent implements OnChanges {
  @Input() threadnum: number = 0;
  @Input() currentstepcount: number = 0;
  @Input() countcalls: number = 0;

  @ViewChild('callstackid') callstackEl!: ElementRef<HTMLDetailsElement>;

  callstack: any[] = [];
  displayedcallstackframes: number = 0;

  private callstackstepcount: number = 0;
  private startcallstackframes: number = 0;
  private readonly maxcallstackframes = 4;
  private readonly bufcallstackframes = 2;

  constructor(private http: HttpClient, public sanitizer: DomSanitizer) {}

  ngOnChanges(changes: SimpleChanges): void {
    if (changes['currentstepcount'] && !changes['currentstepcount'].firstChange) {
      this.callstackstepcount = this.currentstepcount;
      this.startcallstackframes = 0;
      this.displayedcallstackframes = 0;
    }
  }

  trustAsHtml(text: string): SafeHtml {
    return this.sanitizer.bypassSecurityTrustHtml(text);
  }

  loadCallStack(opening: boolean): void {
    const field = this.callstackEl?.nativeElement;
    if (this.countcalls === 0) {
      this.callstack = [];
      this.displayedcallstackframes = 0;
      return;
    }

    if ((opening && !field?.open) || (!opening && field?.open)) {
      if (this.startcallstackframes < this.maxcallstackframes && this.startcallstackframes < this.countcalls) {
        const url = `http://127.0.0.1/callstack.json?threadnum=${this.threadnum}&maxcallstackframes=${this.bufcallstackframes}&startcallstackframes=${this.startcallstackframes}&stepcount=${this.callstackstepcount}`;

        this.http.get<any>(url).subscribe(response => {
          if (response.stepcount === this.callstackstepcount) {
            const records = response.records || [];
            if (this.startcallstackframes === 0) {
              this.callstack = records;
              this.displayedcallstackframes = 0;
            } else {
              this.callstack.push(...records);
            }

            this.displayedcallstackframes += records.length;
            this.startcallstackframes += this.bufcallstackframes;

            // Recursive load more if needed
            if (this.startcallstackframes < this.maxcallstackframes && this.startcallstackframes < this.countcalls) {
              this.loadCallStack(false);
            }
          }
        });
      }
    }
  }
}
